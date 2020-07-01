# UniAutonom RemoteControl GUI

## Requirements
### Build
* C++-Compiler
* Qt 5
* ADTF 2
    * Lade die letzte Version von ADTF2 von [Digitalwerk](https://support.digitalwerk.net/projects/download-center/repository/show/adtf/release-2.14.3) für dein Betriebssystem herunter
        * (derzeit) ohne Anmeldung möglich
        * im Zweifel den Lehrstuhl fragen
    * Installiere ADTF unter beliebigem Pfad
* OpenCV

### Run
* Qt 5

## Build

* Das Projekt verwendet `cmake` als Buildsystem
* Der Pfad zur ADTF-Installation muss als Environmentvariable oder direkt `cmake` als `ADTF_DIR` zur Verfügung gestellt werden
* Als `CMAKE_BUILD_TYPE` `RelWithDebInfo` verwenden

## Run

* Starte ADTF aus dem Praktikumsrepo mit dem Branch `remotecontrol`
* (Optional) Ändere den ListenPort als Property im Filter `TCP Transmitter`
* Initialisiere (noch nicht starten!) den ADTF-Filtergraph
* Starte die Remotecontrol GUI
* Passe ggf. die IP und Port sowie andere Einstellungen unter `Preferences` an. Ggf. GUI neustarten
* Verbinde dich mit ADTF über `Connection -> Connect`
* (Optional) Lade eine Karte über `Map -> Open Map from XML`
* Starte in ADTF die Konfiguration
* Bewege das Fahrzeug. Es sollte sich auch in der Remotecontrol GUI bewegen
