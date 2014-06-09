#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/signal.h>

#define F_S 255

struct structTank
{
		int TankNation;
		int TankType;
		
		struct Rus_Tank
		{
			int HT;
			int MT;
			int LT;
			int TD;
		} RusTank;
			
				
		struct Ger_Tank
		{
			int HT;
			int MT;
			int LT;
			int TD;
		} GerTank;

} Tank;

struct structTank ss(struct structTank Tank) {
	int *atank = (int*)&Tank;
	int count = sizeof(Tank) / sizeof(int);
	struct structTank out;
	int *aout = (int*)&out;
	int i;
	for (i = 0 ; i < count ; i++)
		aout[i] = ntohs(atank[i]);
		
	return out;
}

/* Функция вывода выбранного танка */
struct structTank ShowChosenTank(struct structTank Tank);

int main(int argv, char *argc[])
{

	if (argv != 2)
	{
		printf("Error arguments\n");
		exit(-1);
	}

	int i, j, n;
	int port;
	int msgLength;
	char nickname[25];
	char clan[5];
	
	/* Main (listening) socket */
	int sockfd;
	
	/* Attached socket */
	int newsockfd;
	int testSOCK[255];
	
	port = atoi(argc[1]);

	printf("Server is up and running! To continue press Ctrl+Z and bg \n");
	
	printf ("Введите имя своего клана (заглавные буквы): ");
	scanf ("%s", clan);
	
	/* Main server mode */
	struct sockaddr_in server_addr;
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Error creating socket\n");
		exit(-1);
	}
	
	/* Filling the structure for socket address */
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	/* Getting socket address */
	if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		/* Checking whether socket is busy or not */
		if (errno != 98)
		{
			perror("Error bind(1)\n");
			close(sockfd);
			exit(-1);
		} 
		else 
		{
			server_addr.sin_port = htons(0);
			printf("Port %d is currently unavailable\n", port);
			/* New socket address */
			if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
			{
				perror("Error bind(2)\n");
				close(sockfd);
				exit(-1);
			}
		}
	}
	
	/* Putting created socket into passive (listen) mode */
	if (listen(sockfd, 1) < 0)
	{
		perror("Error listen\n");
		close(sockfd);
		exit(-1);
	}
	
	//------------------------------------------------------------------------
	 /* Структуры для адресов сервера и клиента */
	struct sockaddr_in udpservaddr, udpcliaddr;
	bzero(&udpservaddr, sizeof(udpservaddr));
    udpservaddr.sin_family = AF_INET;
    udpservaddr.sin_port = htons(51000);
    udpservaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	/* Создаем UDP сокет */
	int udpsockfd=socket(PF_INET, SOCK_DGRAM, 0);
	if (udpsockfd<0)
	{
		printf ("Не удалось создать UDP сокет\n");
		exit(1);
	}
	printf ("Success socket \n");
	/* Настраиваем адрес сокета */
	if(bind(udpsockfd, (struct sockaddr *) &udpservaddr,sizeof(udpservaddr)) < 0)
	{
		printf("Не удалось настроить адрес UDP сокета \n");
		close(udpsockfd);
		exit(1);
	}
	
	printf ("Success bind \n");
	
	
	int udpclilen = sizeof(udpcliaddr);

	while(1)
	{
		
		/* Accepting TCP from the client */
		struct sockaddr_in client_addr;
		struct structTank Tank;
		
		/* Max size of client's expected address */
		int clilen = sizeof(client_addr);
		newsockfd=accept(sockfd, (struct sockaddr *) &client_addr, &clilen);
				
		/*New process*/
		pid_t pid;
		if((pid=fork())<0)
		{
			printf("Error fork(1)\n");
			exit(-1);
		}
		
		
		else if (pid==0)
		{
			{
				
				pid=getpid();
				printf ("Pid = %d; NewSockFD = %d\n", pid, newsockfd);			
				/* Receiving message length from the client */
				if ( (msgLength = read(newsockfd, &n, sizeof(int))) < 0 )
				{
					perror("Error receiving message length\n");
					close(sockfd);
					close(newsockfd);
					exit(-1);
				}

				if ((msgLength=read(newsockfd, &Tank, sizeof(Tank))) < 0)
					{
						perror("Error reading the Tank structure\n");
						close(sockfd);
						close(newsockfd);
						exit(-1);
					}
					
				if ((msgLength=read(newsockfd, &nickname, sizeof(nickname))) < 0)
					{
						perror("Error reading the Tank structure\n");
						close(sockfd);
						close(newsockfd);
						exit(-1);
					}
				
				
				Tank = ss(Tank);
				//printf ("Received = %d bits\n", msgLength);
				
				printf("\nИгрок %s[%s] выбрал: ",nickname, clan);
				ShowChosenTank(Tank);
				
				printf ("Введите танк и координаты: ");
				char coordinates[255];
				scanf("%s", coordinates);
				printf ("Coordinates = %s \n", coordinates);
				
				int test=sendto(udpsockfd, coordinates, strlen(coordinates), 0, (struct sockaddr *) &udpcliaddr, udpclilen);
				printf ("Test =%d \n", test);
				if(test< 0)
				{
					printf ("Не удалось отправить координаты \n");
					close(udpsockfd);
					exit(1);
				} 
			}
			//close(newsockfd);
		}
	}
	close(newsockfd);
	return 0;
}

struct structTank ShowChosenTank(struct structTank Tank)
{
	switch(Tank.TankNation)
	{
		//СССР
		case 1: 
			
			switch(Tank.TankType)
			{
				case 1:
					switch(Tank.RusTank.HT)
						{
							case 1:
								printf ("ИС-3\n");
								break;
							case 2:
								printf ("ИС-6\n");
								break;
							case 3:
								printf ("КВ-4\n");
								break;
							case 5:
								printf ("КВ-5\n");
								break;
							default:
								printf ("Неправильный выбор Тяжелого танка СССР\n");
								break;
						}
				break;
				
				case 2:
					switch(Tank.RusTank.MT)
						{
							case 1:
								printf ("Объект 416\n");
								break;
							case 2:
								printf ("Т-44\n");
								break;
							default:
								printf ("Неправильный выбор Среднего танка СССР\n");
								break;
						}
				break;

				case 3:
					switch(Tank.RusTank.LT)
						{
							case 1:
								printf ("МС-1\n");
								break;
							case 2:
								printf ("Т-50\n");
								break;
							case 3:
								printf ("МТ-25\n");
								break;
							default:
								printf ("Неправильный выбор Легкого танка СССР\n");
								break;
						}
				break;	
				
				case 4:
					switch(Tank.RusTank.TD)
						{
							case 1:
								printf ("ИСУ-152\n");
								break;
							case 2:
								printf ("СУ-101\n");
								break;
							default:
								printf ("Неправильный выбор ПТ-САУ\n");
								break;
						}
				break;

				default:
					printf ("Неправильный выбор танка СССР\n");
					break;
			}
		break;
		
		
		//Германия
		case 2: 
			switch(Tank.TankType)
			{
				case 1:
					switch(Tank.GerTank.HT)
						{
							case 1:
								printf ("Tiger II\n");
								break;
							case 2:
								printf ("VK 45.02 (P) Ausf. A\n");
								break;
							case 3:
								printf ("Löwe\n");
								break;
							default:
								printf ("Неправильный выбор Тяжелого танка Германии\n");
								break;
						}
					break;
				
				case 2:
					switch(Tank.GerTank.MT)
						{
							case 1:
								printf ("Indien-Panzer\n");
								break;
							case 2:
								printf ("Panther II\n");
								break;
							default:
								printf ("Неправильный выбор Среднего танка Германии\n");
								break;
						}
					break;
					
				case 3:
					switch(Tank.GerTank.LT)
						{
							case 1:
								printf ("Leichttraktor\n");
								break;
							case 2:
								printf ("Pz.Kpfw. I Ausf. C\n");
								break;
							case 3:
								printf ("VK 28.01\n");
								break;
							case 4:
								printf ("Aufklärungspanzer Panther\n");
								break;
							default:
								printf ("Неправильный выбор Легкого танка Германии\n");
								break;
						}
					break;
				
				case 4:
					switch(Tank.GerTank.TD)
						{
							case 1:
								printf ("Ferdinand\n");
								break;
							case 2:
								printf ("Jagdpanther II\n");
								break;
							case 3:
								printf ("8,8 cm Pak 43 Jagdtiger\n");
								break;	
							case 4:
								printf ("Rhm.-Borsig Waffenträger\n");
								break;	
							default:
								printf ("Неправильный выбор Легкого танка Германии\n");
								break;
						}
					break;
				default: 
					printf ("Неправильный выбор танка Германии\n");
					break;
			}
		break;
		
		default:
			printf ("Неправильный выбор нации \n");
			break;
	}
}
