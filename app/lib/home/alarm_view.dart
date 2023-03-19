import 'package:alarm_clock/client.dart';
import 'package:alarm_clock/home/alarm_editor.dart';
import 'package:flutter/material.dart';

class AlarmView extends StatelessWidget {
  final Alarm alarm;
  final Function(Alarm alarm) updateAlarm;

  const AlarmView({
    super.key,
    required this.alarm,
    required this.updateAlarm,
  });

  _formatTime(int value) {
    return value < 10 ? "0$value" : value.toString();
  }

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(horizontal: 16.0, vertical: 6),
      child: GestureDetector(
        onTap: () async {
          final editedAlarm = await showModalBottomSheet<Alarm?>(
              context: context,
              builder: (context) => AlarmEditor(alarm: alarm));

          if (editedAlarm != null) {
            updateAlarm(editedAlarm);
          }
        },
        child: Container(
          decoration: BoxDecoration(
            borderRadius: BorderRadius.circular(10),
            border: Border.all(color: Colors.white.withOpacity(0.2)),
          ),
          padding: const EdgeInsets.symmetric(
            vertical: 16,
            horizontal: 24,
          ),
          child: Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text(alarm.name),
                  Text(
                    "${_formatTime(alarm.hour)}:${_formatTime(alarm.minute)}",
                    style: TextStyle(fontSize: 40),
                  ),
                ],
              ),
              Switch(
                value: alarm.isEnabled,
                activeColor: Colors.white,
                onChanged: (value) {
                  updateAlarm(alarm.copyWith(isEnabled: !alarm.isEnabled));
                },
              )
            ],
          ),
        ),
      ),
    );
  }
}
