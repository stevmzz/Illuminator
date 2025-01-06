class Componente {
  protected:
    int pin;       // Número de pin del componente
    bool estado;   // Estado del componente (encendido/apagado)
  public:
    // Constructor
    Componente(int numeroPin) {
      pin = numeroPin;
      estado = false;
    }
    // Métodos virtuales
    virtual void iniciar() {
      // Implementación base - puede ser sobrescrita por clases hijas
    }
    virtual void escribir(int valor) {
      // Implementación base - puede ser sobrescrita por clases hijas
    }
    virtual int leer() {
      // Implementación base - puede ser sobrescrita por clases hijas
      return 0;
    }
    // Métodos getter
    int obtenerPin() {
      return pin;
    }
    bool obtenerEstado() {
      return estado;
    }
    // Método setter
    void establecerEstado(bool nuevoEstado) {
      estado = nuevoEstado;
    }
};

// Clase LED
class LED : public Componente {
  public:
    // Constructor
    LED(int numeroPin) : Componente(numeroPin) {}

    // Sobrescritura de métodos
    void iniciar() override {
      // Configurar el pin como salida
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
    }

    void escribir(int valor) override {
      // Escribir valor al LED
      digitalWrite(pin, valor);
      estado = (valor == HIGH);
    }
};

// Clase Sensor
class Sensor : public Componente {
  private:
    const int umbralLuz = 500;  // Umbral para detectar luz suficiente

  public:
    // Constructor
    Sensor(int numeroPin) : Componente(numeroPin) {}

    // Sobrescritura de métodos
    void iniciar() override {
      // Configurar el pin como entrada
      pinMode(pin, INPUT);
    }

    int leer() override {
      // Leer valor analógico del sensor
      return analogRead(pin);
    }
};

// Clase Pantalla
class Pantalla : public Componente {
  public:
    // Constructor
    Pantalla(int numeroPin) : Componente(numeroPin) {}

    // Sobrescritura de métodos
    void iniciar() override {
      // Implementación para inicializar una pantalla
    }

    void escribir(int valor) override {
      // Implementación para escribir valores en la pantalla
    }
};

// Clase Botón
class Boton : public Componente {
  private:
    unsigned long ultimoTiempoBoton = 0;  // Para debounce
    const long tiempoDebounce = 200;      // Tiempo de debounce en ms
    int estadoAnterior = HIGH;            // Estado anterior del botón

  public:
    // Constructor
    Boton(int numeroPin) : Componente(numeroPin) {}

    // Sobrescritura de métodos
    void iniciar() override {
      // Configurar el pin como entrada con pull-up
      pinMode(pin, INPUT_PULLUP);
    }

    int leer() override {
      // Leer estado del botón con debounce
      int estadoActual = digitalRead(pin);
      
      if ((millis() - ultimoTiempoBoton) > tiempoDebounce) {
        if (estadoActual == LOW && estadoAnterior == HIGH) {
          ultimoTiempoBoton = millis();
          estado = !estado;  // Cambiar estado
        }
      }
      estadoAnterior = estadoActual;
      return estado;
    }
};

// Variables globales para el control de iluminación automática
LED* leds[] = {
  new LED(2), new LED(3), new LED(4), new LED(5),  // Grupo 1
  new LED(6), new LED(7), new LED(8)               // Grupo 2
};
Sensor* sensores[] = {new Sensor(A0), new Sensor(A1)};  // Sensores de luz
Boton* botonAutomatico = new Boton(10);                 // Botón modo automático

void setup() {
  Serial.begin(9600);
  
  // Inicializar componentes
  for (int i = 0; i < 7; i++) {
    leds[i]->iniciar();
  }
  for (int i = 0; i < 2; i++) {
    sensores[i]->iniciar();
  }
  botonAutomatico->iniciar();
}

void loop() {
  // Leer estado del modo automático
  bool modoAutomatico = botonAutomatico->leer();
  
  if (modoAutomatico) {
    // Controlar LEDs basado en sensores
    for (int i = 0; i < 4; i++) {  // Grupo 1
      leds[i]->escribir(sensores[0]->leer() >= 500 ? HIGH : LOW);
    }
    for (int i = 4; i < 7; i++) {  // Grupo 2
      leds[i]->escribir(sensores[1]->leer() >= 500 ? HIGH : LOW);
    }
    
    // Debug por Serial
    Serial.print("Sensor 1: ");
    Serial.println(sensores[0]->leer());
    Serial.print("Sensor 2: ");
    Serial.println(sensores[1]->leer());
  }
  
  delay(100);
}