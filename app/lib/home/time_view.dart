import 'dart:async';

import 'package:flutter/material.dart';

class TimeView extends StatefulWidget {
  @override
  State<TimeView> createState() => _TimeViewState();
}

class _TimeViewState extends State<TimeView> {
  late Timer _timer;
  late DateTime _dateTime;

  void _startTimer() {
    const oneSec = const Duration(seconds: 1);

    _timer = new Timer.periodic(
      oneSec,
      (Timer timer) {
        setState(() {
          _dateTime = DateTime.now();
        });
      },
    );
  }

  @override
  void dispose() {
    _timer.cancel();
    super.dispose();
  }

  @override
  void initState() {
    super.initState();
    _startTimer();
    _dateTime = DateTime.now();
  }

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
          crossAxisAlignment: CrossAxisAlignment.center,
          children: [
            Text(
              _dateTime.hour.toString().padLeft(2, '0'),
              style: const TextStyle(fontSize: 80, fontWeight: FontWeight.bold),
            ),
            const Text(":", style: TextStyle(fontSize: 50)),
            Row(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  _dateTime.minute.toString().padLeft(2, '0'),
                  style: const TextStyle(
                      fontSize: 80, fontWeight: FontWeight.bold),
                ),
                Padding(
                  padding: EdgeInsets.only(top: 22),
                  child: Text(
                    _dateTime.second.toString().padLeft(2, '0'),
                    style: const TextStyle(
                        fontSize: 22, fontWeight: FontWeight.bold),
                  ),
                )
              ],
            )
          ],
        ),
        Text(
          "${_getDayOfWeek(_dateTime.weekday)}, ${_dateTime.day}. ${_getNameOfMonth(_dateTime.month)} ${_dateTime.year}",
          style: TextStyle(fontSize: 20, color: Colors.grey.shade400),
        ),
      ],
    );
  }
}
