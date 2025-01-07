// Variables globales
bool modoAutomatico = true;  // Estado del modo automático

class Componente {
  protected:
    int pin; // Número de pin del componente
    bool estado; // Estado del componente

  public:
    // Constructor
    Componente(int numeroPin) {
      pin = numeroPin; // Asigna el número de pin recibido
      estado = false; // Inicialmente, el componente está apagado
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
    
    void iniciar() override {
      pinMode(pin, OUTPUT); // Configura el pin como salida
      digitalWrite(pin, LOW); // Inicialmente apaga el LED
      estado = false; // Actualiza el estado interno
    }
    
    void escribir(int valor) override {
      digitalWrite(pin, valor); // Escribe el valor en el pin sea HIGH o LOW
      estado = (valor == HIGH); // Actualiza el estado
    }
};

// Clase Sensor
class Sensor : public Componente {
  private:
    int umbralLuz; // Umbral para la detección de luz

  public:
    // Constructor
    Sensor(int numeroPin) : Componente(numeroPin) {
      umbralLuz = 5; // Valor predeterminado del umbral ajustado para condiciones de luz
    }

    void iniciar() override {
      pinMode(pin, INPUT); // Configura el pin como entrada
    }

    int leer() override {
      return analogRead(pin); // Lee el valor analógico del sensor
    }

    // Método para establecer el umbral de luz
    void establecerUmbralLuz(int nuevoUmbral) {
      umbralLuz = nuevoUmbral; 
    }

    // Método para obtener el umbral de luz
    int obtenerUmbralLuz() {
      return umbralLuz;
    }
};

// Clase Pantalla
class Pantalla : public Componente {
  private:
    // Aquí puedes agregar variables específicas para la pantalla LCD
    
  public:
    // Constructor
    Pantalla(int numeroPin) : Componente(numeroPin) {}
    
    void iniciar() override {
      // Implementación para inicializar una pantalla
      // Agregar código específico para tu pantalla LCD
    }
    
    void escribir(int valor) override {
      // Implementación para escribir valores en la pantalla
      // Agregar código específico para tu pantalla LCD
    }
};

// Clase Botón
class Boton : public Componente {
  private:
    unsigned long ultimoTiempoBoton; // Para el control de rebote
    const long tiempoDebounce = 50; // Tiempo de debounce en ms
    int estadoAnterior; // Para detectar cambios
    bool esModoAutomatico; // Para diferenciar el tipo de botón

  public:
    // Constructor modificado para especificar el tipo de botón
    Boton(int numeroPin, bool modoAutomatico = false) : Componente(numeroPin) {
      ultimoTiempoBoton = 0;
      estadoAnterior = HIGH;
      esModoAutomatico = modoAutomatico;
    }

    void iniciar() override {
      pinMode(pin, INPUT_PULLUP);
      estadoAnterior = digitalRead(pin);
    }

    int leer() override {
      int estadoActual = digitalRead(pin);
      unsigned long tiempoActual = millis();

      if ((tiempoActual - ultimoTiempoBoton) > tiempoDebounce) {
        if (estadoActual == LOW && estadoAnterior == HIGH) {
          ultimoTiempoBoton = tiempoActual;
          
          if (esModoAutomatico) {
            estado = !estado; // Mantiene el estado para el botón de modo automático
          } else {
            return true; // Para botones manuales, solo retorna true cuando se presiona
          }
        }
      }
      
      estadoAnterior = estadoActual;
      return esModoAutomatico ? estado : false;
    }
};

// Variables para componentes
Sensor sensorLuz(A0);
LED ledsHabitaciones[] = {
    LED(7), // cuearto 1
    LED(8), // patio interno
    LED(9), // Cuarto 2
    LED(10), // patio trasero
    LED(11), // patio delantero
    LED(12), // sala y cocina
    LED(13)  // cuarto 3
};
Boton botonControl(A1, true); // Botón para modo automático
Boton botonesManual[] = {
    Boton(0), // cuarto 1
    Boton(1), // patio interno
    Boton(2), // Cuarto 2
    Boton(3), // patio trasero
    Boton(4), // patio delantero
    Boton(5), // sala y cocina
    Boton(6)  // cuarto 3
};

// Configuración inicial del sistema
void setup() {
    Serial.begin(9600);  // Iniciar comunicación serial para monitoreo

    // Inicializar sensor
    sensorLuz.iniciar();
    sensorLuz.establecerUmbralLuz(5);
    
    // Inicializar botón de modo
    botonControl.iniciar();
    
    // Inicializar LEDs
    for(int i = 0; i < 7; i++) {
        ledsHabitaciones[i].iniciar();
    }
    
    // Inicializar botones de control manual
    for(int i = 0; i < 7; i++) {
        botonesManual[i].iniciar();
    }
}

// Bucle principal del sistema
void loop() {
    // Verificar cambio en el modo automático
    if(botonControl.leer()) {
        modoAutomatico = !modoAutomatico;
        Serial.print("Modo Automático: ");
        Serial.println(modoAutomatico ? "Activado" : "Desactivado");
    }

    // Control automático
    if (modoAutomatico) {
        int valorLuz = sensorLuz.leer();
        for(int i = 0; i < 7; i++) {
            ledsHabitaciones[i].escribir(
                valorLuz > sensorLuz.obtenerUmbralLuz() ? HIGH : LOW
            );
        }
        
        Serial.print("Sensor: ");
        Serial.print(valorLuz);
        Serial.print(" - LEDs: ");
        Serial.println(valorLuz > sensorLuz.obtenerUmbralLuz() ? 
                      "Encendidos" : "Apagados");
    }
    // Control manual
    else {
        // Revisar cada botón y controlar su LED correspondiente
        for(int i = 0; i < 7; i++) {
            if(botonesManual[i].leer()) {
                // Toggle del LED
                bool estadoActual = ledsHabitaciones[i].obtenerEstado();
                ledsHabitaciones[i].escribir(estadoActual ? LOW : HIGH);
                
                // Imprimir estado para depuración
                Serial.print("LED ");
                Serial.print(i);
                Serial.print(": ");
                Serial.println(!estadoActual ? "Encendido" : "Apagado");
            }
        }
    }

    delay(100);  // Pequeña pausa para estabilidad
}