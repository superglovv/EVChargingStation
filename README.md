# Stație de Încărcare EV

## Descrierea Task-ului

Acest proiect simulează o stație de încărcare pentru vehicule electrice utilizând un Arduino Uno. Stația oferă feedback vizual prin intermediul LED-urilor astfel: când stația este disponibilă pentru utilizare, LED-ul RGB luminează verde, după apăsarea butonului de start, stația devine ocupată, iar LED-ul RGB se transformă în roșu, semnalizând că procesul de încărcare a început, iar celelalte LED-uri se aprind secvențial pentru a reprezenta nivelul de încărcare al vehiculului. Printr-o apăsare prelungită (peste 2 secunde) a butonului de stop, procesul de încărcare se oprește, iar LED-ul RGB revine la culoarea verde, semnalizând astfel că stația este din nou disponibilă pentru utilizare.

## Componente Utilizate

- Arduino Uno
- Breadboard
- LED-uri (4x, 2x Albastre, 2x Verzi)
- LED RGB
- Buton START
- Buton STOP
- Rezistențe (6x 330ohm, 1x 220ohm, 2x 1kOhm)
- Cabluri de conectare

## Principiul de Funcționare

### LED RGB – Disponibilitatea Stației

- **Verde**: Stația este liberă și pregătită pentru încărcare.
- **Roșu**: Stația este ocupată și vehiculul este în proces de încărcare.

### LED-uri Individuale – Nivelul de Încărcare

Fiecare LED reprezintă un anumit procent de încărcare a bateriei:

- **L1**: 25%
- **L2**: 50%
- **L3**: 75%
- **L4**: 100%

În timpul încărcării, LED-urile se aprind succesiv, simulând progresul încărcării. LED-ul care indică nivelul curent de încărcare va clipi, iar LED-urile dinainte vor rămâne aprinse continuu.

### Butoane

- **START**: O apăsare scurtă va iniția procesul de încărcare.
- **STOP**: O apăsare lungă (2 secunde sau mai mult) va opri încărcarea și va reseta stația la starea liberă.

## Explicația Codului

### Setarea pinilor și definirea intervalelor de timp și a variabilelor
```cpp
// LED-urile simple și RGB sunt ulterior setate ca ieșiri
#define LED1 10
#define LED2 9
#define LED3 8
#define LED4 7
#define RED_LED 4
#define GREEN_LED 6
#define BLUE_LED 5
#define INTERVAL 3000
//Butoanele START și STOP sunt ulterior setate ca intrări
#define BTNSTART 3
#define BTNSTOP 2
#define BLINK_INTERVAL 200

int lastIntervalTime = 0; // Momentul ultimei iterații a intervalului de timp.
int battery = 0; // Nivelul curent de încărcare a bateriei (0-4).
int charging = 0; // Indică dacă procesul de încărcare este activ (1) sau nu (0).
int buttonPressDuration = 0; // Timpul în care butonul de STOP a fost apăsat.
int lastBlinkTime = 0; // Momentul ultimei schimbări de stare a LED-ului care clipește.
int blinkState = LOW; // Starea curentă a LED-ului (aprins/ stins).
int previousMillis = 0; // Timpul când a început ultima avansare a încărcării.
```

### Funcția de clipire pentru LED-uri
```cpp
// Aceasta funcție face ca LED-urile să clipească la un intervalul fix definit la început (BLINK_INTERVAL)
void blinkLED(int led) { // funcția primeste ca parametru pin-ul LED-ului
  int currentMillis = millis();
  // Se verifică daca a trecut timpul necesar pentru o nouă clipire, iar in caz afirmativ setează timpul curent ca fiind
  // timpul ultimii clipiri și schimbă starea LED-ului
  if (currentMillis - lastBlinkTime >= BLINK_INTERVAL) {
    lastBlinkTime = currentMillis;
    blinkState = !blinkState;
    digitalWrite(led, blinkState);
  }
} 
```

### Funcția de resetare a stației
```cpp
void resetStation() {
  charging = 0; // Pentru a oprii procesul de încarcare
  battery = 0; // Resetează nivelul bateriei
  // Face ca LED-ul RGB să fie verde
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLUE_LED, LOW);

  // Oprește toate LED-urile care reprezintă nivelul de încărcare a bateriei
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
}
```
### Funcția principala LOOP

În aceasta funcție declar variabilele start_button și stop_button pentru cele două butoane de start și stop.

Daca butonul de start este apăsat și charging = 0 se modifică valoarea de charging și se salvează timpul curent (previousMillis) pentru a gestiona timpul de încărcare, iar LED-ul RGB va avea culoarea roșie.
```cpp
if (start_button == LOW && charging == 0) {
    delay(20);
    charging = 1;
    previousMillis = millis();
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
  }
```

Daca butonul de stop este apăsat se verifică durata de apăsare, iar dacă acesta a fost apăsat mai mult de 2 secunde se apelează funcția de resetare a stației.
```cpp
if (stop_button == LOW) {
    if (buttonPressDuration == 0) {
      buttonPressDuration = millis();
    }
    if (millis() - buttonPressDuration >= 2000) {
      resetStation();
      return;
    }
  } else {
    buttonPressDuration = 0;
  }
```
În cazul în care charging = 1 (procesul de încărcare se petrece), se stochează timpul curent în variabila currentMillis, iar dacă timpul scurs de la ultima actualizare este mai mare sau egal cu intervalul de 3 secunde definit la început (INTERVAL), se actualizează previousMillis la timpul curent și se incrementează variabila battery (care reprezintă stadiul de încărcare). Daca toate bateriile s-au incarcat acestea vor clipii de 3 ori, iar charging și battery vor relua valoarea 0 pentru a oprii procesul de încărcare:
```cpp
if (charging) {
    int currentMillis = millis();

    if (currentMillis - previousMillis >= INTERVAL) {
      previousMillis = currentMillis;
      battery++;

      if (battery > 4) {
        battery = 0;
        charging = 0;

        for (int i = 0; i < 3; i++) {
          digitalWrite(LED1, HIGH);
          digitalWrite(LED2, HIGH);
          digitalWrite(LED3, HIGH);
          digitalWrite(LED4, HIGH);
          delay(30);
          digitalWrite(LED1, LOW);
          digitalWrite(LED2, LOW);
          digitalWrite(LED3, LOW);
          digitalWrite(LED4, LOW);
          delay(30);
        }

        resetStation();
        return;
      }
    }
```
În cadrul switch-ului verific procentul curent al bateriei. În funcție de valoarea variabilei battery, se aprinde LED-ul corespunzător stadiului de încărcare curent și, pentru nivelurile intermediare, se apelează funcția blinkLED(LEDx) care simulează încărcarea în curs:
```cpp
  switch (battery) {
      case 0:
        blinkLED(LED1);
        break;
      case 1:
        digitalWrite(LED1, HIGH);
        blinkLED(LED2);
        break;
      case 2:
        digitalWrite(LED2, HIGH);
        blinkLED(LED3);
        break;
      case 3:
        digitalWrite(LED3, HIGH);
        blinkLED(LED4);
        break;
      case 4:
        digitalWrite(LED4, HIGH);
        break;
      default:
        break;
    }
  }
```

## Poze

Pozele pot fi găsite în folder-ul denumit "Poze".

## Video

Videoclipul poate fi văzut prin accesarea link-ului:
[Video pe YouTube](https://youtu.be/XwlkeEq_QTI)

## Schema Circuitului

Pentru a vizualiza schema circuitului în Tinkercad, accesați următorul link:
[Schema circuitului pe Tinkercad](https://www.tinkercad.com/things/fFy8XadlfId-tema-1-ev-charging-station?sharecode=DfYNkKULxrzNeILNhgwix6LQ0-5GoeDqWLfGnqn_l4Y)
