import 'dart:async';
import 'dart:ffi';
import 'dart:io';
import 'dart:math';

import 'package:flutter/foundation.dart';
import 'package:udp/udp.dart';

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

  static simulateRingtone(int trackId) async {
    final request = Request(
      opCode: OpCode.kSimulateRingtone,
    );

    request.buffer.putUint32(trackId, endian: Endian.little);

    final response = await _send(request);
    response.ensureSuccessStatusCode();
  }

  static Future<Response> _send(Request request) async {
    final sender = await _sender;
    final correlationId = Random.secure().nextInt(0xFFFFFFFF);
    final completer = Completer<Response>();
    _pendingRequests[correlationId] = completer;

    final payload = WriteBuffer();
    payload.putUint8List(Uint8List.fromList(_packetMagic));
    payload.putUint8(request.opCode.index);
    payload.putUint32(correlationId, endian: Endian.little);

    final innerPayload = request.buffer.done();
    payload.putUint8List(Uint8List.view(innerPayload.buffer,
        innerPayload.offsetInBytes, innerPayload.lengthInBytes));

    final buffer = payload.done();
    final data = Uint8List.view(
        buffer.buffer, buffer.offsetInBytes, buffer.lengthInBytes);

    await sender.send(
      data,
      Endpoint.unicast(InternetAddress("192.168.4.1"), port: const Port(8999)),
    );

    return await completer.future;
  }
}

enum OpCode {
  kSimulateRingtone,
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
