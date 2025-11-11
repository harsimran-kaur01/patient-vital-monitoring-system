# 🏥 Patient Vital Monitoring System

## 📝 Description

A sophisticated real-time patient vital monitoring system built with C that combines efficient data structures with a modern web interface. This enterprise-grade healthcare application provides medical professionals with instant access to patient data, critical alerts, and comprehensive analytics through a responsive web dashboard.

## 🛠️ Tech Stack
- **Backend**: `C` `Windows Sockets` 
- **Data Structures**: `AVL Trees` `Circular Queues`
- **Frontend**: `HTML5` `CSS3` `JavaScript`
- **Protocol**: `HTTP/1.1`

## ✨ Key Features
- 🎯 **Real-time Monitoring** - Live vital sign tracking
- 🏗️ **AVL Tree Storage** - Optimal O(log n) operations  
- 🚨 **Smart Alerts** - Automatic critical condition detection
- 📊 **Dynamic Dashboard** - Real-time web interface
- 💾 **Memory Efficient** - Optimized data structures

## 📁 Project Structure

```
health
├── compile.bat
├── headers
│   ├── avl_tree.h
│   ├── queue.h
│   ├── vital_monitor.h
│   └── web_server.h
├── patient_monitor.exe
├── sources
│   ├── avl_tree.c
│   ├── main.c
│   ├── queue.c
│   ├── vital_monitor.c
│   └── web_server.c
└── web_pages
    ├── add_record.html
    ├── alerts.html
    ├── index.html
    ├── trends.html
    └── view_records.html
```
## 🎨 System Architecture
```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Web Client    │◄──►│  C Web Server    │◄──►│  Data Structures│
│   (Browser)     │    │   (WinSock)      │    │   (AVL + Queue) │
└─────────────────┘    └──────────────────┘    └─────────────────┘
         │                       │                       │
    HTML/CSS/JS            HTTP Request/Response    Patient Records
    Dynamic Updates                                 Critical Alerts
```
## 📄 License & Disclaimer

### License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

### Medical Disclaimer
**Important**: This software is for educational and demonstration purposes only. It is NOT certified for clinical use and should NOT be used for actual patient monitoring. Always consult healthcare professionals and use certified medical equipment for patient care. See [DISCLAIMER.md](DISCLAIMER.md) for complete details.

## 👥 Contributing

Contributions are welcome! Here's how you can help:

1. **Fork** the repository
2. **Clone** your fork: `git clone https://github.com/your-username/repo.git`
3. **Create** a new branch: `git checkout -b feature/your-feature`
4. **Commit** your changes: `git commit -am 'Add some feature'`
5. **Push** to your branch: `git push origin feature/your-feature`
6. **Open** a pull request

Please ensure your code follows the project's style guidelines and includes tests where applicable.
