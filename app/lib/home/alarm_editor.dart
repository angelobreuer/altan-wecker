import 'package:alarm_clock/client.dart';
import 'package:flutter/material.dart';
import 'package:flutter/rendering.dart';
import 'package:flutter/services.dart';

class _WeekDayButton extends StatelessWidget {
  final WeekDay weekDay;
  final List<WeekDay> weekDays;
  final Function(WeekDay weekDay, bool enabled) updateWeekDay;

  const _WeekDayButton({
    required this.weekDay,
    required this.weekDays,
    required this.updateWeekDay,
  });

  _getWeekDayLiteral(WeekDay weekDay) {
    switch (weekDay) {
      case WeekDay.kMonday:
        return "M";
      case WeekDay.kTuesday:
        return "D";
      case WeekDay.kWednesday:
        return "M";
      case WeekDay.kThursday:
        return "D";
      case WeekDay.kFriday:
        return "F";
      case WeekDay.kSaturday:
        return "S";
      case WeekDay.kSunday:
        return "S";
    }
  }

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 12.0, horizontal: 4),
      child: ElevatedButton(
        style: ButtonStyle(
          padding: MaterialStateProperty.all(const EdgeInsets.symmetric(
            vertical: 20,
            horizontal: 16,
          )),
          minimumSize: MaterialStateProperty.all(Size.zero),
          backgroundColor: MaterialStateProperty.all(
            weekDays.contains(weekDay) ? Colors.green : Colors.transparent,
          ),
          side: MaterialStateProperty.all(
            const BorderSide(
              width: 1,
              color: Colors.white,
            ),
          ),
        ),
        onPressed: () {
          updateWeekDay(weekDay, !weekDays.contains(weekDay));
        },
        child: Text(_getWeekDayLiteral(weekDay)),
      ),
    );
  }
}

class _SectionHeader extends StatelessWidget {
  final String title;
  const _SectionHeader({required this.title});

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.only(top: 24.0),
      child: Text(
        title.toUpperCase(),
        style: const TextStyle(
          fontSize: 15,
          letterSpacing: 1.1,
          color: Colors.grey,
        ),
      ),
    );
  }
}

class AlarmEditor extends StatefulWidget {
  final TextEditingController _controller;
  final Alarm alarm;

  AlarmEditor({super.key, required this.alarm})
      : _controller = TextEditingController(text: alarm.name);

  @override
  State<AlarmEditor> createState() => _AlarmEditorState(alarm: alarm);
}

class _AlarmEditorState extends State<AlarmEditor> {
  Alarm alarm;
  _AlarmEditorState({required this.alarm});

  bool get _isNew => alarm.index == -1;

  void _updateWeekDay(WeekDay weekDay, bool enabled) {
    setState(() {
      if (enabled) {
        alarm = alarm.copyWith(weekDays: [...alarm.weekDays, weekDay]);
      } else {
        final weekDays = [...alarm.weekDays];
        weekDays.remove(weekDay);
        alarm = alarm.copyWith(weekDays: weekDays);
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return Container(
      decoration: const BoxDecoration(
        borderRadius: BorderRadius.only(
          topLeft: Radius.circular(20),
          topRight: Radius.circular(20),
        ),
        color: Colors.black,
      ),
      padding: const EdgeInsets.symmetric(horizontal: 30, vertical: 24),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Text(
            _isNew ? "Alarm erstellen" : "Alarm bearbeiten",
            style: const TextStyle(fontSize: 22, fontWeight: FontWeight.w500),
          ),
          const _SectionHeader(title: "Name"),
          TextField(
            controller: widget._controller,
            maxLength: 32,
            maxLengthEnforcement: MaxLengthEnforcement.enforced,
          ),
          const _SectionHeader(title: "Zeit"),
          SingleChildScrollView(
            scrollDirection: Axis.horizontal,
            child: Row(
              children: [
                Padding(
                  padding: const EdgeInsets.only(right: 8.0),
                  child: ElevatedButton(
                    onPressed: () async {
                      final result = await showTimePicker(
                        context: context,
                        initialTime: TimeOfDay(
                          hour: alarm.hour,
                          minute: alarm.minute,
                        ),
                      );

                      if (result != null && mounted) {
                        setState(() {
                          alarm = alarm.copyWith(
                              hour: result.hour, minute: result.minute);
                        });
                      }
                    },
                    style: ButtonStyle(
                      padding:
                          MaterialStateProperty.all(const EdgeInsets.symmetric(
                        vertical: 16,
                        horizontal: 20,
                      )),
                      minimumSize: MaterialStateProperty.all(Size.zero),
                      backgroundColor:
                          MaterialStateProperty.all(Colors.transparent),
                      side: MaterialStateProperty.all(
                        const BorderSide(
                          width: 1,
                          color: Colors.white,
                        ),
                      ),
                    ),
                    child: Text(
                      TimeOfDay(
                        hour: alarm.hour,
                        minute: alarm.minute,
                      ).format(context),
                      style: const TextStyle(fontSize: 20),
                    ),
                  ),
                ),
                _WeekDayButton(
                    weekDay: WeekDay.kMonday,
                    weekDays: alarm.weekDays,
                    updateWeekDay: _updateWeekDay),
                _WeekDayButton(
                    weekDay: WeekDay.kTuesday,
                    weekDays: alarm.weekDays,
                    updateWeekDay: _updateWeekDay),
                _WeekDayButton(
                    weekDay: WeekDay.kWednesday,
                    weekDays: alarm.weekDays,
                    updateWeekDay: _updateWeekDay),
                _WeekDayButton(
                    weekDay: WeekDay.kThursday,
                    weekDays: alarm.weekDays,
                    updateWeekDay: _updateWeekDay),
                _WeekDayButton(
                    weekDay: WeekDay.kFriday,
                    weekDays: alarm.weekDays,
                    updateWeekDay: _updateWeekDay),
                _WeekDayButton(
                    weekDay: WeekDay.kSaturday,
                    weekDays: alarm.weekDays,
                    updateWeekDay: _updateWeekDay),
                _WeekDayButton(
                    weekDay: WeekDay.kSunday,
                    weekDays: alarm.weekDays,
                    updateWeekDay: _updateWeekDay),
              ],
            ),
          ),
          const _SectionHeader(title: "Weckton"),
          Padding(
            padding: const EdgeInsets.only(right: 8.0, top: 8),
            child: ElevatedButton(
              onPressed: () {
                showTimePicker(
                    context: context,
                    initialTime: const TimeOfDay(hour: 1, minute: 2));
              },
              style: ButtonStyle(
                padding: MaterialStateProperty.all(const EdgeInsets.symmetric(
                  vertical: 16,
                  horizontal: 20,
                )),
                backgroundColor: MaterialStateProperty.all(Colors.transparent),
                side: MaterialStateProperty.all(
                  const BorderSide(
                    width: 1,
                    color: Colors.white,
                  ),
                ),
              ),
              child: Row(
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                children: const [
                  Text("Auswählen"),
                  Icon(Icons.keyboard_arrow_right),
                ],
              ),
            ),
          ),
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Padding(
                padding: const EdgeInsets.only(right: 8.0, top: 30),
                child: ElevatedButton(
                  onPressed: () {
                    Navigator.of(context).pop(null);
                  },
                  style: ButtonStyle(
                    padding: MaterialStateProperty.all(
                      const EdgeInsets.symmetric(
                        vertical: 16,
                        horizontal: 20,
                      ),
                    ),
                    backgroundColor:
                        MaterialStateProperty.all(Colors.transparent),
                    side: MaterialStateProperty.all(
                      const BorderSide(
                        width: 1,
                        color: Colors.red,
                      ),
                    ),
                  ),
                  child: const Text(
                    "Abbruch",
                    style: TextStyle(color: Colors.red),
                  ),
                ),
              ),
              Padding(
                padding: const EdgeInsets.only(right: 8.0, top: 30),
                child: ElevatedButton(
                  onPressed: () {
                    if (alarm.name != widget._controller.text) {
                      alarm = alarm.copyWith(name: widget._controller.text);
                    }

                    Navigator.pop(context, alarm);
                  },
                  style: ButtonStyle(
                    padding: MaterialStateProperty.all(
                      const EdgeInsets.symmetric(
                        vertical: 16,
                        horizontal: 20,
                      ),
                    ),
                    backgroundColor:
                        MaterialStateProperty.all(Colors.transparent),
                    side: MaterialStateProperty.all(
                      const BorderSide(
                        width: 1,
                        color: Colors.white,
                      ),
                    ),
                  ),
                  child: Text(_isNew ? "Erstellen" : "Bearbeiten"),
                ),
              ),
            ],
          ),
        ],
      ),
    );
  }
}
