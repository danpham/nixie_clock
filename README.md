# Nixie Clock ESP32-C3

Un projet de **horloge Nixie basée sur ESP32-C3** avec des tubes IN14 et IN3, pilotée par le driver **HV5622**.

---

## Matériel

- **Microcontrôleur :** ESP32-C3  
- **Tubes Nixie :** 6 × IN14 et 2 × IN3  
- **Driver :** HV5622 pour la commutation des cathodes  
- **Transistors :** NPN haute tension, supportant 300 V  

> ⚠️ Attention : ce projet manipule de la haute tension. Respectez les règles de sécurité électrique !

---

## Logiciel

Le code est écrit en C pour ESP32-C3 et organisé comme suit :

software/nixie_clock_esp32_project/
├── components/
│   ├── uart/
│   ├── gpio_driver/
│   ├── display/
│   ├── clock/
│   └── hv5622/
└── main/

### Fonctionnalités actuelles

- Gestion de l’affichage sur tubes Nixie via HV5622  
- Lecture des boutons rotatifs et encodeurs  
- Horloge interne  

### Fonctionnalités futures

- Connectivité Wi-Fi pour synchronisation automatique de l’heure  
- Interface web pour configurer : fuseau horaire, luminosité, mode nuit, activation/désactivation de l’écran  

---

## Compilation

Le projet utilise **ESP-IDF pour ESP32-C3**. Assurez-vous que votre environnement ESP32-C3 est configuré correctement.

cd software/nixie_clock_esp32_project
idf.py build
idf.py flash

---

## Licence

Ce projet est sous **GPL**.

---

## Sécurité

⚠️ Ce projet implique de la haute tension (~170 V pour les tubes Nixie).  
- Ne jamais toucher le circuit sous tension  
- Utiliser des isolations appropriées  
- Travailler avec précaution et connaissances en électronique HV
