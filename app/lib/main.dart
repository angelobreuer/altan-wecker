import 'package:alarm_clock/client.dart';
import 'package:flutter/material.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      debugShowCheckedModeBanner: false,
      theme: ThemeData.dark(),
      home: const MyHomePage(title: 'Flutter Demo Home Page'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});
  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
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
    return Scaffold(
      backgroundColor: const Color(0xFF17191A),
      appBar: AppBar(
        centerTitle: true,
        elevation: 0,
        backgroundColor: const Color(0xFF17191A),
        title: const Text("Wecker", textAlign: TextAlign.center),
      ),
      body: Center(
        child: Column(
          children: <Widget>[
            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                Text(
                  DateTime.now().hour.toString().padLeft(2, '0'),
                  style: const TextStyle(
                      fontSize: 80, fontWeight: FontWeight.bold),
                ),
                const Text(":", style: TextStyle(fontSize: 50)),
                Text(
                  DateTime.now().minute.toString().padLeft(2, '0'),
                  style: const TextStyle(
                      fontSize: 80, fontWeight: FontWeight.bold),
                ),
              ],
            ),
            Text(
              "${_getDayOfWeek(DateTime.now().weekday)}, ${DateTime.now().day}. ${_getNameOfMonth(DateTime.now().month)} ${DateTime.now().year}",
              style: TextStyle(fontSize: 20, color: Colors.grey.shade400),
            ),
            SizedBox(height: 12),
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: Container(
                decoration: BoxDecoration(
                  borderRadius: BorderRadius.circular(10),
                  color: Colors.red,
                ),
                padding: EdgeInsets.all(16),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.stretch,
                  children: [Text("a")],
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
