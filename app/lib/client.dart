import 'dart:async';
import 'dart:convert';
import 'dart:ffi';
import 'dart:io';
import 'dart:math';

import 'package:enum_flag/enum_flag.dart';
import 'package:flutter/foundation.dart';
import 'package:udp/udp.dart';

enum WeekDay with EnumFlag {
  kMonday,
  kTuesday,
  kWednesday,
  kThursday,
  kFriday,
  kSaturday,
  kSunday,
}

class Alarm {
  final int index;
  final String name;
  final bool isEnabled;
  final List<WeekDay> weekDays;
  final int hour;
  final int minute;
  final int toneId;
  final bool random;

  const Alarm({
    required this.index,
    required this.name,
    required this.isEnabled,
    required this.weekDays,
    required this.hour,
    required this.minute,
    required this.toneId,
    required this.random,
  });

  Alarm copyWith({
    int? index,
    String? name,
    bool? isEnabled,
    List<WeekDay>? weekDays,
    int? hour,
    int? minute,
    int? toneId,
    bool? random,
  }) {
    return Alarm(
      index: index ?? this.index,
      name: name ?? this.name,
      isEnabled: isEnabled ?? this.isEnabled,
      weekDays: weekDays ?? this.weekDays,
      hour: hour ?? this.hour,
      minute: minute ?? this.minute,
      toneId: toneId ?? this.toneId,
      random: random ?? this.random,
    );
  }
}

class Client {
  static final _sender = _createSender();
  static final _packetMagic = [0x88, 0x86, 0x96, 0x77, 0x7F, 0x7F, 0x66];
  static final _pendingRequests = <int, Completer<Response>>{};

  static _createSender() async {
    final sender = await UDP.bind(Endpoint.any());
    sender.asStream().listen(_process);
    return sender;
  }

  static void _process(Datagram? datagram) {
    if (datagram == null || datagram.data.lengthInBytes < 12) {
      return;
    }

    final data = datagram.data;
    final header = Uint8List.view(data.buffer, data.offsetInBytes, 7).toList();

    if (!listEquals(header, _packetMagic)) {
      return;
    }

    final reader = ReadBuffer(ByteData.view(
      data.buffer,
      data.offsetInBytes + 7,
      data.lengthInBytes - 7,
    ));

    final operationStatus = reader.getUint8();
    final correlationId = reader.getUint32(endian: Endian.little);
    final completer = _pendingRequests.remove(correlationId);

    if (completer == null) {
      return;
    }

    final response = Response(
      operationStatus: OperationStatus.values[operationStatus],
      readBuffer: reader,
    );

    completer.complete(response);
  }

  static Future<List<Alarm>> fetchAlarms() async {
    final request = Request(
      opCode: OpCode.kListAlarms,
    );

    final response = await _send(request);
    response.ensureSuccessStatusCode();

    final alarms = List<Alarm>.empty(growable: true);

    while (response.readBuffer.hasRemaining) {
      final index = response.readBuffer.getUint8();
      final name = _getFixedUtf8AsName(response.readBuffer.getUint8List(32));
      final allFlags = response.readBuffer.getUint16(endian: Endian.little);
      final hour = response.readBuffer.getUint8();
      final minute = response.readBuffer.getUint8();
      final toneId = response.readBuffer.getUint16(endian: Endian.little);

      final isEnabled = (allFlags & (1 << 0)) != 0;
      final random = (allFlags & (1 << 1)) != 0;

      final weekDays = List<WeekDay>.empty(growable: true);

      for (var index = 0; index < 7; index++) {
        if (allFlags & (1 << (index + 2)) != 0) {
          weekDays.add(WeekDay.values[index]);
        }
      }

      alarms.add(
        Alarm(
          index: index,
          name: name,
          isEnabled: isEnabled,
          weekDays: weekDays,
          hour: hour,
          minute: minute,
          toneId: toneId,
          random: random,
        ),
      );
    }

    return alarms;
  }

  static Uint8List _getNameAsFixedUtf8(String name) {
    final content = utf8.encode(name);

    if (content.length >= 32) {
      return Uint8List.sublistView(Uint8List.fromList(content), 0, 32);
    }

    final buffer = Uint8List(32);

    for (int index = 0; index < content.length; index++) {
      buffer[index] = content[index];
    }

    return buffer;
  }

  static String _getFixedUtf8AsName(Uint8List name) {
    for (int index = 0; index < 32; index++) {
      if (name[index] == 0) {
        return utf8.decode(Uint8List.sublistView(name, 0, index));
      }
    }

    return utf8.decode(name);
  }

  static updateAlarm(Alarm alarm) async {
    alarm = alarm.copyWith(
        random: true); // TODO: tone selection currently not implemented in app

    final request = Request(
      opCode: OpCode.kUpdateAlarm,
    );

    var flags = alarm.isEnabled ? 1 : 0;

    if (alarm.random) {
      flags |= 1 << 1;
    }

    for (var weekDay in alarm.weekDays) {
      flags |= 1 << (weekDay.index + 2);
    }

    request.buffer.putUint8(alarm.index); // 1
    request.buffer.putUint8List(_getNameAsFixedUtf8(alarm.name)); // 1 + 32
    request.buffer.putUint16(flags, endian: Endian.little); // 1 + 32 + 2
    request.buffer.putUint8(alarm.hour); // 1 + 32 + 2 + 1
    request.buffer.putUint8(alarm.minute); // 1 + 32 + 2 + 1 + 1
    request.buffer.putUint16(alarm.toneId,
        endian: Endian.little); // 1 + 32 + 2 + 1 + 1 + 2

    final response = await _send(request);
    response.ensureSuccessStatusCode();
  }

  static simulateRingtone(int trackId) async {
    final request = Request(
      opCode: OpCode.kSimulateRingtone,
    );

    request.buffer.putUint32(trackId, endian: Endian.little);

    final response = await _send(request);
    response.ensureSuccessStatusCode();
  }

  static ping() async {
    final request = Request(opCode: OpCode.kPing);
    final response = await _send(request);
    response.ensureSuccessStatusCode();
  }

  static Future<void> _sendInternal(
      int correlationId, Request request, ByteData innerPayload) async {
    final sender = await _sender;
    final payload = WriteBuffer();
    payload.putUint8List(Uint8List.fromList(_packetMagic));
    payload.putUint8(request.opCode.index);
    payload.putUint32(correlationId, endian: Endian.little);

    payload.putUint8List(Uint8List.view(innerPayload.buffer,
        innerPayload.offsetInBytes, innerPayload.lengthInBytes));

    final buffer = payload.done();
    final data = Uint8List.view(
        buffer.buffer, buffer.offsetInBytes, buffer.lengthInBytes);

    await sender.send(
      data,
      Endpoint.broadcast(
          /*InternetAddress("192.168.178.177"),*/
          port: const Port(8999)),
    );
  }

  static Future<Response> _send(Request request) async {
    final correlationId = Random.secure().nextInt(0xFFFFFFFF);
    final innerPayload = request.buffer.done();
    final completer = Completer<Response>();
    _pendingRequests[correlationId] = completer;

    await _sendInternal(correlationId, request, innerPayload);

    final timer = Timer.periodic(const Duration(seconds: 1), (timer) {
      _sendInternal(correlationId, request, innerPayload);
    });

    try {
      return await completer.future;
    } finally {
      timer.cancel();
    }
  }
}

enum OpCode {
  kSimulateRingtone,
  kListAlarms,
  kUpdateAlarm,
  kPing,
}

enum OperationStatus {
  kSuccess,
  kErrorUnspecified,
  kBadRequest,
}

class Response {
  final OperationStatus operationStatus;
  final ReadBuffer readBuffer;

  void ensureSuccessStatusCode() {
    if (operationStatus != OperationStatus.kSuccess) {
      throw Exception("Non-success status code.");
    }
  }

  const Response({
    required this.operationStatus,
    required this.readBuffer,
  });
}

class Request {
  final OpCode opCode;
  final WriteBuffer buffer;

  Request({
    required this.opCode,
  }) : buffer = WriteBuffer();
}
