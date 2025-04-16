#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>

#define PAIS1_ID 1
#define PAIS2_ID 2
#define CAPACIDAD_CARGA 1000
#define UMBRAL_MEDIO 750
#define UMBRAL_BAJO 450

//DA ERROR MIRAR PQ
//sigset_t mask, oldmask;

int pipe1[2], pipe2[2];

volatile sig_atomic_t sigusr1_recibida = 0;

void sigusr1(int signo){
  if (signo == SIGUSR1){
    sigusr1_recibida = 1;
  }
}

//POR AHORA SON VOID POR PONER ALGO, NSE SI NECESITARAN MAS PARAMETROS O RETURNEAR ALGO
//PENSAR SI LOS READ I WRITE SE HAN DE METER EN IF POR SI HAY ERROR LEYENDO O ESCRIBIENDO
//Funcion para los procesos hijos
void processo_hijo(int N){
  int i;
  double limite, extracion;
  //Cerrar extremos de tuberia
  //De pipe1 solo queremos leer y de pipe2 solo queremos escribir
  close(pipe1[1]);
  close(pipe2[0]);

  //Inicializar semilla del generador de numeros aleatorios srand
  srand(getpid());

  //Ejecutar bucle N iteraciones
  for (i = 0; i < N; i++)
  {
    //Esperamos que el processo reciba la senyal SI
    while(!sigusr1_recibida){};
    sigusr1_recibida = 0;

    //Leemos el limite de extraccion enviado por el padre
    read(pipe1[0], &limite, sizeof(double));

    //Generamos la cantidad aleatoria a extraer
    extracion = ((double)rand() / RAND_MAX) * limite;

    //Transmitimos la extraccion generada al padre por pipe2
    write(pipe2[1], &extracion, sizeof(double));
  }
};
// Calcula la recuperación anual de recursos basada en condiciones aleatorias
// Recibe: recursos_actuales (cantidad disponible actual)
// Retorna: cantidad a incrementar (puede ser cero o negativa en casos adversos)
double calcular_recuperacion(double recursos_actuales) {
  double porcentaje, porcentaje_base, variacion;
  int evento;

  // Determina el porcentaje base según el nivel de recursos
  if (recursos_actuales <= UMBRAL_MEDIO && recursos_actuales > UMBRAL_BAJO) {
      porcentaje_base = 20;  // Recursos moderados: recuperación alta
  } else {
      porcentaje_base = 5;   // Recursos muy altos o muy bajos: recuperación baja
  }

  // Genera un evento aleatorio (1-100) usando el PID como semilla
  srand(getpid());
  evento = rand() % 100 + 1;

  // Asigna variación según probabilidades
  if (evento < 75) {
      variacion = 0;  // 75% de probabilidad: condiciones normales
      printf("Este año se han producido condiciones normales de recuperación");
  } else if (evento >= 75 && evento < 90) {
      variacion = -15;  // 15% de probabilidad: condiciones adversas
      printf("Este año se han producido condiciones adversas de recuperación");
  } else {
      variacion = 10;  // 10% de probabilidad: condiciones favorables
      printf("Este año se han producido condiciones favorables de recuperación");
  }

  // Calcula el porcentaje final (no menor a cero)
  porcentaje = max(porcentaje_base + variacion, 0);
  printf("El porcentaje de recuperación es del: %.6f\n", porcentaje);

  // Retorna la cantidad a incrementar (porcentaje aplicado a recursos actuales)
  return (recursos_actuales * (porcentaje / 100));
}
//Funcion para el proceso padre
// Gestiona la simulación anual de extracción de recursos para dos países
// Parámetros: PID de los hijos, años de simulación (N), y límites de extracción
void processo_padre(pid_t pid1, pid_t pid2, int N, double lim_alto, double lim_medio, double lim_bajo) {
  int i;
  double limite, solicitud_pais1, solicitud_pais2, recursos_recuperados, recursos_aux;
  double recursos_disponibles = 1000, extraccion_pais1 = 0, extraccion_pais2 = 0;

  // Cierra extremos no usados de las tuberías
  close(pipe1[0]);  // Padre no lee de pipe1
  close(pipe2[1]);  // Padre no escribe en pipe2

  // Bucle principal por cada año de simulación
  for (i = 0; i < N; i++) {
      printf("* AÑO %d\n", i + 1);
      printf("Los recursos disponibles para el año en curso son %.6f\n", recursos_disponibles);

      // Asigna límite de extracción según umbrales
      if (recursos_disponibles > UMBRAL_MEDIO) {
          limite = lim_alto;
          printf("El límite de extracción para el año en curso es %.6f\n", lim_alto);
      } else if (recursos_disponibles <= UMBRAL_MEDIO && recursos_disponibles > UMBRAL_BAJO) {
          limite = lim_medio;
          printf("El límite de extracción para el año en curso es %.6f\n", lim_medio);
      } else {
          limite = lim_bajo;
          printf("El límite de extracción para el año en curso es %.6f\n", lim_bajo);
      }

      // Proceso para País 1
      kill(pid1, SIGUSR1);  // Señal para iniciar turno
      write(pipe1[1], &limite, sizeof(double));  // Envía límite
      read(pipe2[0], &solicitud_pais1, sizeof(double));  // Lee solicitud
      printf("El país 1 solicita extraer %.6f\n", solicitud_pais1);

      // Ajusta solicitud si supera recursos disponibles
      if (solicitud_pais1 > recursos_disponibles) {
          printf("La solicitud de extracción del país 1 no se ha aprobado, se va a extraer el posible\n");
          solicitud_pais1 = recursos_disponibles;
      } else {
          printf("La solicitud de extracción del país 1 se ha aprobado\n");
      }
      recursos_disponibles -= solicitud_pais1;
      extraccion_pais1 += solicitud_pais1;

      // Proceso para País 2 (similar al País 1)
      kill(pid2, SIGUSR1);
      write(pipe1[1], &limite, sizeof(double));
      read(pipe2[0], &solicitud_pais2, sizeof(double));
      printf("El país 2 solicita extraer %.6f\n", solicitud_pais2);

      if (solicitud_pais2 > recursos_disponibles) {
          printf("La solicitud de extracción del país 2 no se ha aprobado, se va a extraer el posible\n");
          solicitud_pais2 = recursos_disponibles;
      } else {
          printf("La solicitud de extracción del país 2 se ha aprobado\n");
      }
      recursos_disponibles -= solicitud_pais2;
      extraccion_pais2 += solicitud_pais2;

      // Calcula recuperación anual
      recursos_aux = recursos_disponibles;
      recursos_recuperados = calcular_recuperacion(recursos_disponibles);
      recursos_disponibles = min(recursos_disponibles + recursos_recuperados, CAPACIDAD_CARGA);

      // Ajuste si se alcanza la capacidad máxima
      if (recursos_disponibles == CAPACIDAD_CARGA) {
          recursos_recuperados = CAPACIDAD_CARGA - recursos_aux;
      }
      printf("Los recursos se recuperaron en %.6f unidades\n", recursos_recuperados);
  }

  // Resultados finales
  printf("La simulación ha finalizado.\n");
  printf("Recursos extraídos por país 1: %.6f\n", extraccion_pais1);
  printf("Recursos extraídos por país 2: %.6f\n", extraccion_pais2);
  printf("Total recursos extraídos: %.6f\n", (extraccion_pais1 + extraccion_pais2));
  printf("Recursos disponibles: %.6f\n", recursos_disponibles);
}

int main(int argc, char *argv[])
{
      // Leer parámetros

  // Limites de la carga
  double lim_alto, lim_medio, lim_bajo;
  int N;
  pid_t pid1, pid2;



  if (argc != 5)
  {
    printf("%s <lim_alto> <lim_medio> <lim_bajo> <N>\n", argv[0]);
    exit(1);
  }

  lim_alto = atof(argv[1]);
  lim_medio = atof(argv[2]);
  lim_bajo = atof(argv[3]);
  N = atoi(argv[4]);

  if (N < 1)
  {
    printf("N no pot ser zero o negatiu!\n");
    exit(1);
  }

  if (lim_alto < 0 || lim_medio < 0 || lim_bajo < 0)
  {
    printf("Els límits no poden ser negatius!\n");
    exit(1);
  }

  //Creacion de tuberias
  //Pipe1 Processo padre envia información a los hijos
  pipe(pipe1);
  //Pipe 2 Processos hijos envian información al padre
  pipe(pipe2);

  //Se registra la funcion para manejar la señal SIGURS1
  signal(SIGUSR1, sigusr1);

  //Creamos el primer proceso hijo
  pid1 = fork();
  if (pid1 == 0){
    processo_hijo(N);
  }
  //Creamos el segundo procesjo hijo
  pid2 = fork();
  if(pid2 == 0){
    processo_hijo(N);
  }





      // TO-BE-DONE

  return 0;
}
