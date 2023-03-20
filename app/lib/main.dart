import 'package:alarm_clock/client.dart';
import 'package:alarm_clock/home/alarm_list.dart';
import 'package:alarm_clock/home/time_view.dart';
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
      body: SingleChildScrollView(
        child: Center(
          child: Column(
            children: <Widget>[
              TimeView(),
              const SizedBox(height: 30),
              AlarmList(),
              ElevatedButton(
                  onPressed: () {
                    Client.simulateRingtone(30);
                  },
                  child: Text("a")),
            ],
          ),
        ),
      ),
    );
  }
}
