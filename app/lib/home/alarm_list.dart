import 'package:alarm_clock/client.dart';
import 'package:alarm_clock/home/add_alarm_button.dart';
import 'package:alarm_clock/home/alarm_view.dart';
import 'package:flutter/material.dart';

class AlarmList extends StatefulWidget {
  const AlarmList({super.key});

  @override
  State<AlarmList> createState() => _AlarmListState();
}

class _AlarmListState extends State<AlarmList> {
  List<Alarm>? _alarms;

  void _loadAlarms() async {
    final alarms = await Client.fetchAlarms();
    setState(() => _alarms = alarms);
  }

  @override
  void initState() {
    super.initState();
    _loadAlarms();
  }

  _buildInnerList() {
    final alarms = _alarms;

    if (alarms != null) {
      return alarms.map((e) => AlarmView(
          alarm: e,
          updateAlarm: (alarm) async {
            await Client.updateAlarm(alarm);
            _loadAlarms();
          }));
    }

    return const [CircularProgressIndicator()];
  }

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        ..._buildInnerList(),
        AddAlarmButton(),
      ],
    );
  }
}
