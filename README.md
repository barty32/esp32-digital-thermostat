
# Digitální termostat s ESP32

Cílem projektu je nahradit stávající domácí termostat na ovládání ústředního topení, který je
zastaralý a kvůli analogové regulaci teploty i dost nepřesný. Nový termostat bude měřit teplotu
digitálně a zároveň bude předvídat budoucí teplotu, aby byla regulace co nejpřesnější. Dále má
zjednodušit nastavování časových slotů pro regulaci teploty díky bezdrátovému ovládání přes
lokální WiFi síť.

Plynový kotel, ke kterému je termostat připojen, se ovládá pomocí třípinového (tzv. Junkers 1-2-4)
rozhraní, kde pin 1 je napájecí napětí +24 V DC, pin 4 je zem (GND) a pin 2 slouží k plynulému
řízení výkonu kotle v závislosti na napětí (3 V – 0 %, 21 V – 100 %). Díky tomu je možné spínat
kotel méně často a tím se prodlužuje jeho životnost.

## Schéma zapojení

![schéma](/img/schema_bb.jpg)

Teplota je měřena čidlem DS18B20 a digitálně posílána do mikrokontroléru. Výstup pro řízení kotle
je generován interním DAC převodníkem a zesílen na 24 V operačním zesilovačem LM358.
Napájení obvodu je řešeno lineárním regulátorem, který v budoucnu vyměním za step-down měnič
kvůli větší účinnosti. Zařízení se ovládá pěti tlačítky (nahoru, dolů, doleva, doprava a mode),
uživatelské rozhraní je zobrazeno na alfanumerickém 16x2 LCD displeji.

## Popis kódu

Kód se skládá ze dvou částí – webové aplikace (podsložka /client), přes kterou lze ovládat
termostat po síti, a samotného firmwaru, který řídí všechno ostatní.
Webová aplikace je napsána ve frameworku React v jazyce Typescript. Zobrazuje aktuální teplotu,
status kotle (zap/vyp), historii teploty za posledních 24 hodin a nastavení teploty a časových slotů.
Tyto sloty lze také upravovat. Aplikace komunikuje s termostatem přes vlastní JSON REST API.

Důležité C++ třídy v kódu firmwaru:
- ScreenManager: stará se o vykreslování uživatelského rozhraní na LCD obrazovku
- ButtonLib: načítá a vyhodnocuje stavy tlačítek (včetně debouncingu)
- ThermostatController: samotné jádro regulace teploty v závislosti na vstupních parametrech
- Terminal: rozhraní pro komunikaci přes sériový port
- Day, Time, Temperature: pomocné třídy pro snadnější manipulaci s daty

Zbytek funkcionality (Web server, WiFi konfigurace, RTC) je implementován v souborech
main.cpp a util.cpp.

## Sestavený prototyp

![prototyp](/img/prototyp.jpg)
