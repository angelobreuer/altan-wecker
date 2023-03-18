import 'dart:convert';
import 'dart:math';

import 'package:alarm_clock/client.dart';
import 'package:flutter/material.dart';

class AddAlarmButton extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(horizontal: 16.0, vertical: 6),
      child: GestureDetector(
        onTap: () async {
          /*await Client.updateAlarm(Alarm(
            hour: 7,
            index: 0,
            minute: 0,
            name: "Abc",
            isEnabled: true,
            toneIsCategory: true,
            toneId: 0,
            weekDays: [WeekDay.kFriday],
          ));*/

          /*final alarms = await Client.fetchAlarms();
          print('a');*/

          await Client.simulateRingtone(Random().nextInt(255));
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
