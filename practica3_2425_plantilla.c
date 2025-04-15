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

//Funcion para el proceso padre
void  processo_padare(){};

int main(int argc, char *argv[])
{
      // Leer parámetros

  // Limites de la carga
  int lim_alto, lim_medio, lim_bajo, N;
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

  if(lim_alto > 1000 || lim_medio > 1000 || lim_bajo > 1000)
  {
    printf("Els límits no poden ser superiors a 1000!\n");
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
