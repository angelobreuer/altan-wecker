import 'package:alarm_clock/client.dart';
import 'package:alarm_clock/home/alarm_editor.dart';
import 'package:flutter/material.dart';

class AddAlarmButton extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(horizontal: 16.0, vertical: 6),
      child: GestureDetector(
        onTap: () async {
          await showModalBottomSheet(
            context: context,
            builder: (context) => AlarmEditor(
              alarm: Alarm(
                index: -1,
                name: "Neuer Alarm",
                isEnabled: true,
                weekDays: WeekDay.values,
                hour: 6,
                minute: 0,
                toneId: 0,
                toneIsCategory: false,
              ),
            ),
            backgroundColor: Colors.transparent,
          );
        },
        child: Container(
          decoration: BoxDecoration(
            borderRadius: BorderRadius.circular(11),
            border: Border.all(color: Colors.white.withOpacity(0.2)),
          ),
          padding: const EdgeInsets.symmetric(
            vertical: 22,
            horizontal: 24,
          ),
          child: Row(
            crossAxisAlignment: CrossAxisAlignment.center,
            mainAxisAlignment: MainAxisAlignment.center,
            children: const [
              Icon(Icons.add),
              SizedBox(width: 4),
              Text(
                "Neuen Wecker hinzufügen",
                style: TextStyle(fontSize: 16),
                textAlign: TextAlign.center,
              ),
            ],
          ),
        ),
      ),
    );
  }
}
