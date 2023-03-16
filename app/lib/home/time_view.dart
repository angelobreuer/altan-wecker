import 'package:flutter/material.dart';

class TimeView extends StatelessWidget {
  _getDayOfWeek(int weekday) {
    switch (weekday) {
      case DateTime.monday:
        return "Montag";
      case DateTime.tuesday:
        return "Dienstag";
      case DateTime.wednesday:
        return "Mittwoch";
      case DateTime.thursday:
        return "Donnerstag";
      case DateTime.friday:
        return "Freitag";
      case DateTime.saturday:
        return "Samstag";
      case DateTime.sunday:
        return "Sonntag";
    }
  }

  _getNameOfMonth(int month) {
    switch (month) {
      case 1:
        return "Januar";
      case 2:
        return "Februar";
      case 3:
        return "März";
      case 4:
        return "April";
      case 5:
        return "Mai";
      case 6:
        return "Juni";
      case 7:
        return "Juli";
      case 8:
        return "August";
      case 9:
        return "September";
      case 10:
        return "Oktober";
      case 11:
        return "November";
      case 12:
        return "Dezember";
    }
  }

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        Row(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text(
              DateTime.now().hour.toString().padLeft(2, '0'),
              style: const TextStyle(fontSize: 80, fontWeight: FontWeight.bold),
            ),
            const Text(":", style: TextStyle(fontSize: 50)),
            Text(
              DateTime.now().minute.toString().padLeft(2, '0'),
              style: const TextStyle(fontSize: 80, fontWeight: FontWeight.bold),
            ),
          ],
        ),
        Text(
          "${_getDayOfWeek(DateTime.now().weekday)}, ${DateTime.now().day}. ${_getNameOfMonth(DateTime.now().month)} ${DateTime.now().year}",
          style: TextStyle(fontSize: 20, color: Colors.grey.shade400),
        ),
      ],
    );
  }
}
