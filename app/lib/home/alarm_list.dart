import 'package:alarm_clock/home/add_alarm_button.dart';
import 'package:alarm_clock/home/alarm_view.dart';
import 'package:flutter/material.dart';

class AlarmList extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        Alarm(),
        Alarm(),
        Alarm(),
        AddAlarmButton(),
      ],
    );
  }
}
