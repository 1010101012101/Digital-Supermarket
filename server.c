#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <float.h>


#define OPTION_MAX_SIZE 1
#define NAME_MIN_SIZE 6
#define NAME_MAX_SIZE 50
#define USERNAME_MIN_SIZE 6
#define USERNAME_MAX_SIZE 20
#define PASSWORD_MIN_SIZE 6
#define PASSWORD_MAX_SIZE 50
#define EMAIL_MAX_SIZE 50
#define CONTACT_SIZE_1 9
#define CONTACT_SIZE_2 12
#define MONEY_MAX_SIZE_STRING 42      //340282346638528859811704183484516925440.00 tem 42 caracteres 
#define MONEY_MAX_SIZE_FLOAT 340282346638528859811704183484516925440.00
#define QUANTITY_MAX_SIZE_STRING 10
#define QUANTITY_MAX_SIZE_INT 2147483647
#define PERMS_MAX_SIZE 1
#define PERMS_MAX_SIZE_STRING 5      //ADMIN = 5 caracteres
#define CODE_SIZE 9
#define CONFIRMATION_MAX_SIZE 3
#define DAY_MAX_SIZE 2
#define MONTH_MAX_SIZE 2
#define YEAR_MAX_SIZE 4
#define FILE_MAX_SIZE 37      //8 ("userdb\") + 20 (username) + 8 (shoplist)
#define LOGGED_MAX_SIZE 5      //FALSE é o maior

/* Notas: 1 - Nas variáveis que contenham os nomes, usernames etc acima descritas, temos que meter mais 2 espaços
para verificação de erros e para variáveis onde vamos ler de ficheiros, é só mais 1 */


static char loggedUsername[USERNAME_MAX_SIZE+1];     //variável global para ver qual é o username que está com sessão iniciada

//autenticação
bool auth();
void logUser(char username[]);
void logoutUser(char username[]);

//menus
void menu();      //menu principal
void menuManageUser();      //menu de gestão de utilizadores
void menuChangeUser();      //menu de modificar propriedades dos utilizadores
void menuManageProduct();      //menu de gestão de stock
void menuChangeProduct();      //menu de modificar propriedades dos produtos em stock
void menuStats();      //menu de estatisticas

//verificações: não foi posto a impressão de erros nas verificações por causa da palavra "Novo"
bool checkNewOption(char option[]);      //verificar uma opção introduzida
bool checkNewName(char username[]);      //verificar um novo nome
bool checkNewUsername(char username[]);      //verificar um novo username
bool checkNewPass(char password[]);      //verificar uma nova password
bool checkNewEmail(char email[]);      //verificar um novo e-mail
bool checkNewContact(char contact[]);      //verificar um novo contacto
bool checkNewBalance(char stringBalance[]);      //verificar um novo saldo de conta bancária
bool checkNewPerms(char perms[]);      //verificar novas permissões
bool checkConfirmation(char confirmation[]);      //verificar confirmações
bool checkNewCode(char code[]);      //verificar códigos de produtos
bool checkNewQuantity(char stringQuantity[]);      //verificar quantidades de produtos
bool checkDay(char day[]);      //verificar o dia de uma data
bool checkMonth(char month[]);      //verificar mes de uma data
bool checkYear(char year[]);


//gerir utilizadores
struct userData {      //estrutura das propriedades de um utilizador
	char name[NAME_MAX_SIZE+1];      //+1 para o \0'
	char username[USERNAME_MAX_SIZE+1];
	char password[PASSWORD_MAX_SIZE+1];
	char email[EMAIL_MAX_SIZE+1];
	char contact[CONTACT_SIZE_2+1];
	float balance;
	char perms[PERMS_MAX_SIZE_STRING+1];
	char logged[LOGGED_MAX_SIZE+1];
};
void searchUser();      //listar todos os utilizadores existentes
void addUser();      //adicionar um novo utilizador
void changeName();      //modificar o nome de um utilizador
void changeContact();      //modificar o contactp de um utilizador
void changeEmail();      //modificar o e-mail de um utilizador
void changePass();      //modificar a password de um utilizador
void changeBalance();      //modificar o saldo de uma conta bancária de um utilizador
void changePerms();      //modificar as permissões de um utilizador
void deleteUser();      //eliminar um utilizador

//gerir produtos
struct productData {      //estrutura das propriedades de um produto
	char code[CODE_SIZE+1];      //+1 para o '\0'
	char name[NAME_MAX_SIZE+1];
	int quantity;
	float price;
};
void searchProduct();      //listar todos os produtos existentes
void addProduct();      //adicionar um novo produto ao stock
void changeProductCode();      //modificar o código de um produto
void changeProductName();      //modificar o nome de um produto
void changeProductAvailableQuantity();      //modificar a quantidade disponivel de um produto
void changeProductPrice();      //modificar o preço de um produto
void deleteProduct();      //eliminar um produto

//estatisticas
struct salesData {
	char code[CODE_SIZE+1];
	int quantity;
	char username[USERNAME_MAX_SIZE+1];
	char day[DAY_MAX_SIZE+1];
	char month[MONTH_MAX_SIZE+1];
	char year[YEAR_MAX_SIZE+1];
};
void productStats();      //estatisticas de um produto
void topProductStats();      //TOP estatísticas de todos os produtos vendidos
void topProductUserStats();      //TOP estatísticas de produtos comprados por um utilizador
void dailyStats();      //estatisticas diárias de vendas
void topDailyStats();      //TOP produtos vendidos num determinado dia
void monthlyStats();      //estatisticas mensais de vendas
void topMonthlyStats();      //TOP produtos vendidos num determinado mes

//funções auxiliares
//estrutura das propriedades de um ficheiro (propriedades que é preciso passar em várias funções)
struct fileStructure {
	int fd;
	struct flock *lock;
	FILE *filePointer;
};

/*abrir um ficheiro especificado com os modos READ, WRITE ou APPEND (ler, escrever ou acrescentar ao fim, respetivamente)
e meter um lock de leitura, escrita ou escrita para mais ninguem poder aceder ao ficheiro
Lock de leitura = quem quiser pode ler mas nã\o escrever
Lock de escrita = só quem tem o lock pode escrever e ninguem pode ler
Lock de acrescentar = Lock de escrita*/
struct fileStructure openFile(char fileToEdit[], char mode[]);
void closeFile(struct fileStructure fileStruct);      //fechar e retirar o lock do ficheiro antes aberto

/* https://bytes.com/topic/c/answers/726898-checking-excessive-input-when-using-fgets */
void flushInput();      //limpar o buffer de input (maneira em que dá para todos os sistemas operativos)
bool getString(char string[], int size);      //fgets mas eliminar o que resta no buffer stdin


void generateCode(char code[]);      //gerar código de produto aleatório

void cleanScreen();      //limpar o ecra (meter vários ENTER's ( '\n''s ))
void removeNewLine(char string[]);      //trocar o ENTER ('\n') pelo null ('\0') numa string para dar menos erros a comparar strings
float stringToFloat(char stringBalance[]);      //converter strings para floats
int stringToInt(char stringQuantity[]);

void about();      //imprimir informações sobre o programa



int main(){

	//criar o diretório "usersdb" caso não exista e caso exista ele não vai ser criado
	mkdir("usersdb", 0766);      //mkdir = comando para criar diretórios, 0 = octal, 7 = RWX(user), 6 = RW(group + otherusers)

	/*em primeiro, abrir os ficheiros "userdb", "productdb" e "salesdb" e se eles não existirem, criar-los*/
	//abrir o ficheiro em modo de acrescentar para que se ele não existir, criar-lo
	struct fileStructure file_userdb = openFile("userdb", "READ");
	
	if (!file_userdb.filePointer)
		return -1;

	//se não existir nenhum utilizador com permissões de administrador, criar um com o nome admin e pass admin
	char filePerms[PERMS_MAX_SIZE_STRING+1];
	bool admin = false;
	while (fscanf(file_userdb.filePointer, "%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*f;%[^;];%*s\n", filePerms)!=EOF){
		if ( !strcmp(filePerms, "ADMIN")){
			admin=true;
			break;
		}
	}
	closeFile(file_userdb);

	if ( !admin){
		file_userdb = openFile("userdb", "APPEND");
		
		if (!file_userdb.filePointer)
			return -1;
		
		fprintf(file_userdb.filePointer, "admin;admin;admin;admin@admin.com;123456789;0.00;ADMIN;FALSE\n");
		closeFile(file_userdb);

		//variaveis para concatenar
		char createDir[FILE_MAX_SIZE]="usersdb/";      //8 (usersdb/) + 20 (USERNAME_MAX_SIZE) + 1 ('\0')
		strcat(createDir, "admin");

		char createShoplist[FILE_MAX_SIZE]="usersdb/";      //28 = diretório + 8 (shoplist) + 1 ('\0')
		strcat(createShoplist, "admin");
		strcat(createShoplist, "/");
		strcat(createShoplist, "shoplist");
		
		//criar diretório de utilizador
		mkdir(createDir, 0766);      //mkdir = comando para criar diretórios

		//criar ficheiro de lista de compras
		struct fileStructure file_shoplist = openFile(createShoplist, "READ");
		if (!file_shoplist.filePointer)
			return -1;
		closeFile(file_shoplist);
	}
	
	//criar ficheiro se não existir
	struct fileStructure file_productdb = openFile("productdb", "READ");
	if (!file_productdb.filePointer)
		return -1;

	closeFile(file_productdb);

	//criar ficheiro se não existir
	struct fileStructure file_salesdb = openFile("salesdb", "READ");
	if (!file_salesdb.filePointer)
		return -1;
	
	closeFile(file_salesdb);




	fputs("-----Servidor de Gestão do SuperMercado-----\n\n", stdout);
	fputs("Recomendação: Usar em Fullscreen\n\n", stdout);
	fputs("\n-----Autenticação-----\n\n", stdout);

	//enquanto o utilizador não se autenticar, estar sempre a pedir para se autenticar
	while(!auth());

	//caso ele se autentique, passar para o menu
	menu();

	//retornar 0 para saber se o programa terminou bem
	return 0;
}

bool auth(){
	//+2 para um caracter a mais para determinar se a string obedece a todas as propriedades e para o '\0'
	char username[USERNAME_MAX_SIZE+2], password[PASSWORD_MAX_SIZE+2];

	fputs("Username: ", stdout);
	getString(username, USERNAME_MAX_SIZE+2);
	fputs("Password: ", stdout);
	getString(password, PASSWORD_MAX_SIZE+2);


	/*abrir o ficheiro userdb, meter um lock (modo de leitura) no ficheiro e passar
	as informações para a estrutura file_userdb*/
	struct fileStructure file_userdb = openFile("userdb", "READ");

	//se o ficheiro não conseguiu ser aberto, dar erro e sair desta função
	if (!file_userdb.filePointer)
		return false;

	/*declarar variáveis a ser usadas nas operações de ficheiros 
	(só é declarado aqui em baixo para poupar memória caso o ficheiro não consiga ser aberto)*/
	char fileUsername[USERNAME_MAX_SIZE+1], filePassword[PASSWORD_MAX_SIZE+1], filePerms[PERMS_MAX_SIZE_STRING+1], fileLogged[LOGGED_MAX_SIZE+1];      //+1 para '\0'

	//enquanto não lermos o ficheiro todo, continuar a ler
	/*  %[^;]; = ler qualquer coisa separado por ;
	    %*[^;]; ignorar (*) qualquer coisa separado por ; 
	http://stackoverflow.com/questions/20450333/fscanf-with-colon-delimited-data*/
	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%[^;];%*[^;];%*[^;];%*f;%[^;];%s\n", fileUsername, filePassword, filePerms, fileLogged)!=EOF){

		if ( !strcmp(fileUsername, username)) {      //se o utilizador existir
			if ( !strcmp(filePassword, password)) {      //se o utilizador existir e a password estiver certa
				if ( !strcmp(filePerms, "ADMIN")){      //se o utilizador existir, a password estiver certa e tiver permissões de administrador
					if ( !strcmp(fileLogged, "FALSE")){      //se o utilizador existir, a password estiver certa, ter permissoes de admin e ainda não se ter autenticado
						cleanScreen();
						closeFile(file_userdb);
						strcpy(loggedUsername, username);
						logUser(username);
						fputs("Autenticado com sucesso!\n\n\n", stdout);
						return true;
					}
					else {      //se o utilizador existir, a password estiver certa, ter permissoes de admin mas já estar autenticado
						closeFile(file_userdb);
						fputs("\n\n\nErro! Este utilizador já está autenticado.\n\n\n", stdout);
						return false;
					}
				}
				else {      //se o utilizador existir, a password estiver certa mas não tiver permissões de administrador
					closeFile(file_userdb);
					fputs("\n\n\nErro! Não tem permissões para aceder ao programa de gestão.\n\n", stdout);
					return false;
				}

			}
			else {      //se o utilizador existir e a password NÃO estiver certa
				closeFile(file_userdb);
				fputs("\n\n\nErro! Password errada. Por-favor tente outra vez.\n\n", stdout);
				return false;
			}
		}
	}

	//se o utilizador não existir
	//fechar e tirar o lock do ficheiro
	closeFile(file_userdb);
	fputs("\n\n\nErro! Utilizador não encontrado. Por-favor tente outra vez.\n\n", stdout);
	return false;
}

void menu(){

	char option[OPTION_MAX_SIZE+2]="0";

	while(option[0]!='5'){
		fputs("-----Menu-----\n\n", stdout);
		fputs("1) Gerir Utilizadores\n", stdout);
		fputs("2) Gerir Stock\n", stdout);
		fputs("3) Ver Estatísticas\n", stdout);
		fputs("4) Sobre\n", stdout);
		fputs("5) Logout\n", stdout);
		fputs("\nEscolha: ", stdout);
		getString(option, OPTION_MAX_SIZE+2);

		if ( !checkNewOption(option))
			option[0]='a';      /*como todas as opções sao algarismos, isto vai fazer 
								com que a opção não seja reconhecida*/

		switch(option[0]){
			case '1': cleanScreen();
				menuManageUser();
				break;
			case '2': cleanScreen();
				menuManageProduct();
				break;
			case '3': cleanScreen();
				menuStats();
				break;
			case '4': cleanScreen();
				about();
				break;
			case '5': cleanScreen();
				logoutUser(loggedUsername);
				fputs("Obrigado por manter o SuperMercado a funcionar corretamente!\n", stdout);
				break;
			default: cleanScreen();
				fputs("Erro! Opção não reconhecida.\n\n\n", stdout);
				break;
		}
	}
}

void menuManageUser(){

	char option[OPTION_MAX_SIZE+2]="0";

	while(option[0]!='5'){
		fputs("-----Gestão de Utilizadores-----\n\n", stdout);
		fputs("1) Procurar utilizadores\n", stdout);
		fputs("2) Adicionar um utilizador\n", stdout);
		fputs("3) Modificar um utilizador\n", stdout);
		fputs("4) Eliminar um utilizador\n", stdout);
		fputs("5) Voltar ao menu principal\n", stdout);
		fputs("\nEscolha: ", stdout);
		getString(option, OPTION_MAX_SIZE+2);

		if ( !checkNewOption(option))
			option[0]='a';

		switch(option[0]){
			case '1': cleanScreen();
				searchUser();
				break;
			case '2': cleanScreen();
				addUser();
				break;
			case '3': cleanScreen();
				menuChangeUser();
				break;
			case '4': cleanScreen();
				deleteUser();
				break;
			case '5': cleanScreen();
				break;
			default: cleanScreen();
				fputs("Erro! Opção não reconhecida.\n\n\n", stdout);
				break;
		}
	}
}

void searchUser(){
	char username[USERNAME_MAX_SIZE+2];

	fputs("-----Pesquisar Utilizadores-----\n\n\n", stdout);
	fputs("Padrão dos usernames dos utilizadores a pesquisar: (ENTER para listar todos os utilizadores) ", stdout);
	getString(username, USERNAME_MAX_SIZE+2);

	struct fileStructure file_userdb = openFile("userdb", "READ");

	if (!file_userdb.filePointer)
		return;


	cleanScreen();      //limpar o ecra para ficar bonito

	struct userData tempUserData;

	int i, j;
	bool pattern, found_at_least_one=false;
	while (fscanf(file_userdb.filePointer, "%[^;];%[^;];%*[^;];%[^;];%[^;];%f;%[^;];%s\n", tempUserData.name, tempUserData.username,
		tempUserData.email, tempUserData.contact, &tempUserData.balance, tempUserData.perms, tempUserData.logged)!=EOF){
		
		for (i=0; tempUserData.username[i]!='\0'; i++){
			if (tempUserData.username[i]==username[0]){      /*se a letra que estiver no index i da palavra tempUserData.username 
													for igual à primeira letra da palavra username*/
				pattern = true;
				for (j=1; tempUserData.username[i]!='\0' && username[j]!='\0'; j++){      /*como alguma letra do tempUserData.username é igual à 
																				primeira letra do username que queremos pesquisar, 
																				entrar noutro ciclo para percorrer o tempUserData.username 
																				e a pesquisa ao mesmo tempo*/
					if (tempUserData.username[i+j]!=username[j]){
						pattern=false;
						break;
					}
				}
			}
			else if (username[0]=='\0'){
				pattern=true;
				i=0;      //i=0 e j=0 para não dar Segmentation Fault no teste de imprimir as coisas quando o username[0]=='\0'
				j=0;
			}
			else if (username[0]!='\0')
				pattern=false;

			if (pattern){
				if ((tempUserData.username[i+j]=='\0' && username[j]=='\0') || (tempUserData.username[i+j]!='\0' && username[j]=='\0') || (username[0]=='\0')){
					found_at_least_one=true;
					fprintf(stdout, "Nome: %s\n", tempUserData.name);
					fprintf(stdout, "Username: %s\n", tempUserData.username);
					fprintf(stdout, "E-Mail: %s\n", tempUserData.email);
					fprintf(stdout, "Contacto telefónico: %s\n", tempUserData.contact);
					fprintf(stdout, "Saldo da conta bancária: %.2f €\n", tempUserData.balance);
					fprintf(stdout, "Permissões: %s\n", tempUserData.perms);
					fprintf(stdout, "Autenticado: %s\n\n\n", tempUserData.logged);
					break;
				}
			}
		}
	}

	if ( !found_at_least_one)
		fprintf(stdout, "Não foi encontrado nenhum utilizador que tivesse \"%s\" contido no username.\n\n\n", username);

	closeFile(file_userdb);
}

void addUser(){

	char name[NAME_MAX_SIZE+2], contact[CONTACT_SIZE_2+2] , email[EMAIL_MAX_SIZE+2], username[USERNAME_MAX_SIZE+2],
	password[PASSWORD_MAX_SIZE+2], perms[PERMS_MAX_SIZE+2], stringBalance[MONEY_MAX_SIZE_STRING+2];

	fputs("-----Criar Utilizador-----\n\n\n", stdout);
	fputs("* = obrigatório ser original\n\n", stdout);
	fputs("Nome completo (não pode conter ';' e tem que ter entre 6 a 50 caracteres): ", stdout);
	getString(name, NAME_MAX_SIZE+2);

	//verificar se todos os campos são válidos, senão sair sem escrever para o ficheiro
	if (!checkNewName(name)){
		cleanScreen();
		fputs("Erro! Nome não válido.\n\n\n", stdout);
		return;
	}

	fputs("Username (6 a 20 caracteres e não são permitidos ';' nem espaços)*: ", stdout);
	getString(username, USERNAME_MAX_SIZE+2);
	if (!checkNewUsername(username)){
		cleanScreen();
		fputs("Erro! Username não válido.\n\n\n", stdout);
		return;
	}

	
	fputs("Password (6 a 50 caracteres e não são permitidos ';' nem espaços): ", stdout);
	getString(password, PASSWORD_MAX_SIZE+2);
	if (!checkNewPass(password)){
		cleanScreen();
		fputs("Erro! Password não válida.\n\n\n", stdout);
		return;
	}


	fputs("Endereço de e-mail: (não pode conter ';' e ter no máximo 50 caracteres)*: ", stdout);
	getString(email, EMAIL_MAX_SIZE+2);
	if (!checkNewEmail(email)){
		cleanScreen();
		fputs("Erro! Endereço de e-mail não válido.\n\n\n", stdout);
		return;
	}


	fputs("Contacto telefónico (conter 9 ou 12 números)*: ", stdout);
	getString(contact, CONTACT_SIZE_2+2);
	if (!checkNewContact(contact)){
		cleanScreen();
		fputs("Erro! Contacto telefónico não válido.\n\n\n", stdout);
		return;
	}


	fputs("Saldo inicial da conta bancária (entre 0.0 e 340282346638528859811704183484516925440.0, até 2 casas decimais e usar '.' em vez de ','): ", stdout);
	getString(stringBalance, MONEY_MAX_SIZE_STRING+2);
	if (!checkNewBalance(stringBalance)){
		cleanScreen();
		fputs("Erro! Saldo inicial da conta bancária não válido.\n\n\n", stdout);
		return;
	}


	fputs("Permissões -> 1) Administrador, 2) Utilizador: ", stdout);
	getString(perms, PERMS_MAX_SIZE+2);
	if (!checkNewPerms(perms)){
		cleanScreen();
		fputs("Erro! Opção de permissões não válida.\n\n\n", stdout);
		return;
	}


	//abrir o ficheiro e meter um lock (modo de acrescentar)
	struct fileStructure file_userdb = openFile("userdb", "APPEND");

	if (!file_userdb.filePointer)
		return;

	char fileUsername[USERNAME_MAX_SIZE+1], fileEmail[EMAIL_MAX_SIZE+1], fileContact[CONTACT_SIZE_2+1];
	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%*[^;];%[^;];%[^;];%*f;%*[^;];%*s\n", fileUsername, fileEmail, fileContact)!=EOF){
		if ( !strcmp(fileUsername, username)) {
			closeFile(file_userdb);
			cleanScreen();
			fprintf(stdout, "Erro! O utilizador com o username \"%s\" já existe.\n\n\n", username);
			return;
		}
		else if ( !strcmp(fileEmail, email)) {
			closeFile(file_userdb);
			cleanScreen();
			fprintf(stdout, "Erro! O utilizador com o e-mail \"%s\" já existe.\n\n\n", email);
			return;
		}
		else if ( !strcmp(fileContact, contact)) {
			closeFile(file_userdb);
			cleanScreen();
			fprintf(stdout, "Erro! O utilizador com o contacto telefónico\"%s\" já existe.\n\n\n", contact);
			return;
		}
	}

	float balance = stringToFloat(stringBalance);

	fprintf(file_userdb.filePointer, "%s;%s;%s;%s;%s;%f;", name, username, password, email, contact, balance);
	if (perms[0]=='1')
		fputs("ADMIN;", file_userdb.filePointer);
	else if (perms[0]=='2')
		fputs("USER;", file_userdb.filePointer);
	fprintf(file_userdb.filePointer, "FALSE\n");

	closeFile(file_userdb);


	//variaveis para concatenar
	char createDir[FILE_MAX_SIZE+1]="usersdb/";      //8 (usersdb/) + 20 (USERNAME_MAX_SIZE) + 1 ('\0')
	strcat(createDir, username);

	//criar diretório de utilizador
    mkdir(createDir, 0766);      //mkdir = comando para criar diretórios


	char createShoplist[FILE_MAX_SIZE+1]="usersdb/";      //28 = diretório + 8 (shoplist) + 1 ('\0')
	strcat(createShoplist, username);
	strcat(createShoplist, "/");
	strcat(createShoplist, "shoplist");

    //criar ficheiro de lista de compras
    struct fileStructure file_create = openFile(createShoplist, "READ");
	if (!file_create.filePointer)
		return;
	closeFile(file_create);
	
	
	//limpar o ecra
	cleanScreen();
	fprintf(stdout, "Utilizador \"%s\" criado com sucesso!\n\n\n", username);
}

void menuChangeUser(){

	char option[OPTION_MAX_SIZE+2]="0";

	while(option[0]!='7'){
		fputs("-----Modificar Utilizadores-----\n\n", stdout);
		fputs("1) Mudar o nome de um utilizador\n", stdout);
		fputs("2) Mudar password de um utilizador\n", stdout);
		fputs("3) Mudar o e-mail de um utilizador\n", stdout);
		fputs("4) Mudar o contacto telefónico de um utilizador\n", stdout);
		fputs("5) Mudar conta bancária de um utilizador\n", stdout);
		fputs("6) Mudar permissões de um utilizador\n", stdout);
		fputs("7) Voltar ao menu de gestão de utilizadores\n", stdout);
		printf("\nEscolha: ");
		getString(option, OPTION_MAX_SIZE+2);

		if ( !checkNewOption(option))
			option[0]='a';

		switch(option[0]){
			case '1': cleanScreen();
				changeName();
				break;
			case '2': cleanScreen();
				changePass();
				break;
			case '3': cleanScreen();
				changeEmail();
				break;
			case '4': cleanScreen();
				changeContact();
				break;
			case '5': cleanScreen();
				changeBalance();
				break;
			case '6': cleanScreen();
				changePerms();
				break;
			case '7': cleanScreen();
				break;
			default: cleanScreen();
				fputs("Erro! Opção não reconhecida.\n\n\n", stdout);
				break;
		}
	}
}

void changeName(){

	char username[USERNAME_MAX_SIZE+2];

	fputs("-----Mudar Nome de um Utilizador-----\n\n", stdout);

	fputs("Qual é o username do utilizador que deseja modificar? ", stdout);
	getString(username, USERNAME_MAX_SIZE+2);


	struct fileStructure file_userdb = openFile("userdb", "READ");

	if (!file_userdb.filePointer)
		return;

	char fileName[NAME_MAX_SIZE+1], fileUsername[USERNAME_MAX_SIZE+1];
	bool found=false;

	while (fscanf(file_userdb.filePointer, "%[^;];%[^;];%*[^;];%*[^;];%*[^;];%*f;%*[^;];%*s\n", fileName, fileUsername)!=EOF){
		if ( !strcmp(fileUsername, username)) {
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_userdb);
		cleanScreen();
		fprintf(stdout, "Erro! O utilizador com o username \"%s\" não foi encontrado.\n\n\n", username);
		return;
	}

	closeFile(file_userdb);


	char newName[NAME_MAX_SIZE+2];

	fprintf(stdout, "\n\n\nNome atual do utilizador \"%s\": %s\n\n", fileUsername, fileName);
	fputs("Novo nome (não pode conter ';' e tem que ter entre 6 a 50 caracteres): ", stdout);
	getString(newName, NAME_MAX_SIZE+2);

	if (!checkNewName(newName)){
		cleanScreen();
		fputs("Erro! Novo nome não válido.\n\n\n", stdout);
		return;
	}


	file_userdb = openFile("userdb", "WRITE");

	if (!file_userdb.filePointer)
		return;

	struct fileStructure file_temp = openFile("temp", "APPEND");

	if (!file_temp.filePointer){
		closeFile(file_userdb);
		return;
	}


	//estrutura para meter as informações do ficheiro
	struct userData tempData;

	while(fscanf(file_userdb.filePointer, "%[^;];%[^;];%[^;];%[^;];%[^;];%f;%[^;];%s\n", tempData.name, tempData.username, 
			tempData.password, tempData.email, tempData.contact, &tempData.balance, tempData.perms, tempData.logged) != EOF){

		if ( !strcmp(tempData.username, username))
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", newName, tempData.username, tempData.password, 
				tempData.email, tempData.contact, tempData.balance, tempData.perms, tempData.logged);

		else
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, tempData.password, 
				tempData.email, tempData.contact, tempData.balance, tempData.perms, tempData.logged);
	}


	rename("temp", "userdb");
	closeFile(file_userdb);
	closeFile(file_temp);
	cleanScreen();
	fprintf(stdout, "O nome do utilizador com o username \"%s\" foi alterado com sucesso!\n\n\n", username);
}

void changePass(){

	char username[USERNAME_MAX_SIZE+2];

	fputs("-----Mudar Password de um Utilizador-----\n\n", stdout);

	fputs("Qual é o username do utilizador que deseja modificar? ", stdout);
	getString(username, USERNAME_MAX_SIZE+2);

	struct fileStructure file_userdb = openFile("userdb", "READ");

	if (!file_userdb.filePointer)
		return;

	char fileUsername[USERNAME_MAX_SIZE+1];
	bool found=false;

	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%*[^;];%*[^;];%*[^;];%*f;%*[^;];%*s\n", fileUsername)!=EOF){
		if ( !strcmp(fileUsername, username)) {
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_userdb);
		cleanScreen();
		fprintf(stdout, "Erro! O utilizador com o username \"%s\" não foi encontrado.\n\n\n", username);
		return;
	}

	closeFile(file_userdb);


	char newPass[PASSWORD_MAX_SIZE+2];

	//aqui não é dito a password antiga do utilizador para "ninguem" saber
	fputs("\n\nNova password (6 a 50 caracteres e não são permitidos ';' nem espaços): ", stdout);
	getString(newPass, PASSWORD_MAX_SIZE+2);

	if (!checkNewPass(newPass)){
		cleanScreen();
		fputs("Erro! Nova password não válida.\n\n\n", stdout);
		return;
	}


	file_userdb = openFile("userdb", "WRITE");

	if (!file_userdb.filePointer)
		return;

	struct fileStructure file_temp = openFile("temp", "APPEND");

	if (!file_temp.filePointer){
		closeFile(file_userdb);
		return;
	}


	//estrutura para meter as informações do ficheiro
	struct userData tempData;

	while(fscanf(file_userdb.filePointer, "%[^;];%[^;];%[^;];%[^;];%[^;];%f;%[^;];%s\n", tempData.name, tempData.username, 
			tempData.password, tempData.email, tempData.contact, &tempData.balance, tempData.perms, tempData.logged) != EOF){

		if ( !strcmp(tempData.username, username))
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, newPass, 
				tempData.email, tempData.contact, tempData.balance, tempData.perms, tempData.logged);

		else
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, tempData.password, 
				tempData.email, tempData.contact, tempData.balance, tempData.perms, tempData.logged);
	}


	rename("temp", "userdb");
	closeFile(file_userdb);
	closeFile(file_temp);
	cleanScreen();
	fprintf(stdout, "A password do utilizador com o username \"%s\" foi alterado com sucesso!\n\n\n", username);
}

void changeEmail(){

	char username[USERNAME_MAX_SIZE+2];

	fputs("-----Mudar E-Mail de um Utilizador-----\n\n", stdout);

	fputs("Qual é o username do utilizador que deseja modificar? ", stdout);
	getString(username, USERNAME_MAX_SIZE+2);


	struct fileStructure file_userdb = openFile("userdb", "READ");

	if (!file_userdb.filePointer)
		return;

	char fileUsername[USERNAME_MAX_SIZE+1], fileEmail[EMAIL_MAX_SIZE+1];
	bool found=false;

	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%*[^;];%[^;];%*[^;];%*f;%*[^;];%*s\n", fileUsername, fileEmail)!=EOF){
		if ( !strcmp(fileUsername, username)) {
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_userdb);
		cleanScreen();
		fprintf(stdout, "Erro! O utilizador com o username \"%s\" não foi encontrado.\n\n\n", username);
		return;
	}

	closeFile(file_userdb);


	char newEmail[EMAIL_MAX_SIZE+2];

	fprintf(stdout, "\n\n\nE-Mail atual do utilizador \"%s\": %s\n\n", fileUsername, fileEmail);
	fputs("* = obrigatório ser original\n\n", stdout);
	fputs("Novo e-mail (não pode conter ';' e ter no máximo 50 caracteres)*: ", stdout);
	getString(newEmail, EMAIL_MAX_SIZE+2);

	if (!checkNewEmail(newEmail)){
		cleanScreen();
		fputs("Erro! Novo e-mail não válido.\n\n\n", stdout);
		return;
	}



	file_userdb = openFile("userdb", "WRITE");

	if (!file_userdb.filePointer)
		return;


	//ver se já existe algum utilizador com o mesmo e-mail
	while (fscanf(file_userdb.filePointer, "%*[^;];%*[^;];%*[^;];%[^;];%*[^;];%*f;%*[^;];%*s\n", fileEmail)!=EOF){
		if ( !strcmp(fileEmail, newEmail)) {
			closeFile(file_userdb);
			cleanScreen();
			fprintf(stdout, "Erro! O utilizador com o e-mail \"%s\" já existe.\n\n\n", newEmail);
			return;
		}
	}


	struct fileStructure file_temp = openFile("temp", "APPEND");

	if (!file_temp.filePointer){
		closeFile(file_userdb);
		return;
	}


	//estrutura para meter as informações do ficheiro
	struct userData tempData;

	fseek(file_userdb.filePointer, 0, SEEK_SET);
	while(fscanf(file_userdb.filePointer, "%[^;];%[^;];%[^;];%[^;];%[^;];%f;%[^;];%s\n", tempData.name, tempData.username, 
			tempData.password, tempData.email, tempData.contact, &tempData.balance, tempData.perms, tempData.logged) != EOF){

		if ( !strcmp(tempData.username, username))
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, tempData.password, 
				newEmail, tempData.contact, tempData.balance, tempData.perms, tempData.logged);

		else
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, tempData.password, 
				tempData.email, tempData.contact, tempData.balance, tempData.perms, tempData.logged);
	}


	rename("temp", "userdb");
	closeFile(file_userdb);
	closeFile(file_temp);
	cleanScreen();
	fprintf(stdout, "O e-mail do utilizador com o username \"%s\" foi alterado com sucesso!\n\n\n", username);
}

void changeContact(){

	char username[USERNAME_MAX_SIZE+2];

	fputs("-----Mudar Contacto Telefónico de um Utilizador-----\n\n", stdout);

	fputs("Qual é o username do utilizador que deseja modificar? ", stdout);
	getString(username, USERNAME_MAX_SIZE+2);


	struct fileStructure file_userdb = openFile("userdb", "READ");

	if (!file_userdb.filePointer)
		return;

	char fileUsername[USERNAME_MAX_SIZE+1], fileContact[CONTACT_SIZE_2+1];
	bool found=false;

	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%*[^;];%*[^;];%[^;];%*f;%*[^;];%*s\n", fileUsername, fileContact)!=EOF){
		if ( !strcmp(fileUsername, username)) {
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_userdb);
		cleanScreen();
		fprintf(stdout, "Erro! O utilizador com o username \"%s\" não foi encontrado.\n\n\n", username);
		return;
	}

	closeFile(file_userdb);


	char newContact[CONTACT_SIZE_2+2];

	fprintf(stdout, "\n\n\nContacto telefónico atual do utilizador \"%s\": %s\n\n", fileUsername, fileContact);
	fputs("* = obrigatório ser original\n\n", stdout);
	fputs("Novo contacto telefónico (conter 9 ou 12 números)*: ", stdout);
	getString(newContact, CONTACT_SIZE_2+2);

	if (!checkNewContact(newContact)){
		cleanScreen();
		fputs("Erro! Novo contacto telefónico não válido.\n\n\n", stdout);
		return;
	}


	file_userdb = openFile("userdb", "WRITE");

	if (!file_userdb.filePointer)
		return;


	//ver se já existe algum utilizador com o mesmo contacto
	while (fscanf(file_userdb.filePointer, "%*[^;];%*[^;];%*[^;];%*[^;];%[^;];%*f;%*[^;];%*s\n", fileContact)!=EOF){
		if ( !strcmp(fileContact, newContact)) {
			closeFile(file_userdb);
			cleanScreen();
			fprintf(stdout, "Erro! O utilizador com o contacto telefónico \"%s\" já existe.\n\n\n", newContact);
			return;
		}
	}


	struct fileStructure file_temp = openFile("temp", "APPEND");

	if (!file_temp.filePointer){
		closeFile(file_userdb);
		return;
	}


	//estrutura para meter as informações do ficheiro
	struct userData tempData;

	fseek(file_userdb.filePointer, 0, SEEK_SET);
	while(fscanf(file_userdb.filePointer, "%[^;];%[^;];%[^;];%[^;];%[^;];%f;%[^;];%s\n", tempData.name, tempData.username, 
			tempData.password, tempData.email, tempData.contact, &tempData.balance, tempData.perms, tempData.logged) != EOF){

		if ( !strcmp(tempData.username, username))
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, tempData.password, 
				tempData.email, newContact, tempData.balance, tempData.perms, tempData.logged);

		else
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, tempData.password, 
				tempData.email, tempData.contact, tempData.balance, tempData.perms, tempData.logged);
	}


	rename("temp", "userdb");
	closeFile(file_userdb);
	closeFile(file_temp);
	cleanScreen();
	fprintf(stdout, "O contacto telefónico do utilizador com o username \"%s\" foi alterado com sucesso!\n\n\n", username);
}

void changeBalance(){

	char username[USERNAME_MAX_SIZE+2];

	fputs("-----Mudar Montande de Conta Bancária de um Utilizador-----\n\n", stdout);

	fputs("Qual é o username do utilizador que deseja modificar? ", stdout);
	getString(username, USERNAME_MAX_SIZE+2);


	struct fileStructure file_userdb = openFile("userdb", "READ");

	if (!file_userdb.filePointer)
		return;

	char fileUsername[USERNAME_MAX_SIZE+1];
	float userBalance;
	bool found=false;

	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%*[^;];%*[^;];%*[^;];%f;%*[^;];%*s\n", fileUsername, &userBalance)!=EOF){
		if ( !strcmp(fileUsername, username)) {
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_userdb);
		cleanScreen();
		fprintf(stdout, "Erro! O utilizador com o username \"%s\" não foi encontrado.\n\n\n", username);
		return;
	}

	closeFile(file_userdb);


	char newStringBalance[MONEY_MAX_SIZE_STRING+2];

	fprintf(stdout, "\n\n\nSaldo da conta bancária atual do utilizador \"%s\": %.2f €\n\n", fileUsername, userBalance);
	fputs("Novo saldo (entre 0.0 e 340282346638528859811704183484516925440.0, até 2 casas decimais e usar '.' em vez de ','): ", stdout);
	getString(newStringBalance, MONEY_MAX_SIZE_STRING+2);

	if (!checkNewBalance(newStringBalance)){
		cleanScreen();
		fputs("Erro! Novo saldo não válido.\n\n\n", stdout);
		return;
	}


	//transformar a string com o novo saldo para um float
	float newBalance = stringToFloat(newStringBalance);


	file_userdb = openFile("userdb", "WRITE");

	if (!file_userdb.filePointer)
		return;

	struct fileStructure file_temp = openFile("temp", "APPEND");

	if (!file_temp.filePointer)
		return;


	//estrutura para meter as informações do ficheiro
	struct userData tempData;

	while(fscanf(file_userdb.filePointer, "%[^;];%[^;];%[^;];%[^;];%[^;];%f;%[^;];%s\n", tempData.name, tempData.username, 
			tempData.password, tempData.email, tempData.contact, &tempData.balance, tempData.perms, tempData.logged) != EOF){

		if ( !strcmp(tempData.username, username))
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, tempData.password, 
				tempData.email, tempData.contact, newBalance, tempData.perms, tempData.logged);

		else
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, tempData.password, 
				tempData.email, tempData.contact, tempData.balance, tempData.perms, tempData.logged);
	}


	rename("temp", "userdb");
	closeFile(file_userdb);
	closeFile(file_temp);
	cleanScreen();
	fprintf(stdout, "O saldo da conta bancária do utilizador com o username \"%s\" foi alterado com sucesso!\n\n\n", username);
}

void changePerms(){

	char username[USERNAME_MAX_SIZE+2];

	fputs("-----Mudar Permissões de um Utilizador-----\n\n", stdout);

	fputs("Qual é o username do utilizador que deseja modificar? ", stdout);
	getString(username, USERNAME_MAX_SIZE+2);


	struct fileStructure file_userdb = openFile("userdb", "READ");

	if (!file_userdb.filePointer)
		return;

	char fileUsername[USERNAME_MAX_SIZE+1], filePerms[EMAIL_MAX_SIZE+1];
	bool found=false;

	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%*[^;];%*[^;];%*[^;];%*f;%[^;];%*s\n", fileUsername, filePerms)!=EOF){
		if ( !strcmp(fileUsername, username)) {
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_userdb);
		cleanScreen();
		fprintf(stdout, "Erro! O utilizador com o username \"%s\" não foi encontrado.\n\n\n", username);
		return;
	}

	closeFile(file_userdb);


	char newPerms[PERMS_MAX_SIZE+2];

	fprintf(stdout, "\nPermissoes atuais do utilizador \"%s\": %s\n\n\n", fileUsername, filePerms);
	fputs("Novas permissões -> 1) Administrador, 2) Utilizador: ", stdout);
	getString(newPerms, PERMS_MAX_SIZE+2);

	if (!checkNewPerms(newPerms)){
		cleanScreen();
		fputs("Erro! Novas permissões não válidas.\n\n\n", stdout);
		return;
	}


	file_userdb = openFile("userdb", "WRITE");

	if (!file_userdb.filePointer)
		return;

	struct fileStructure file_temp = openFile("temp", "APPEND");

	if (!file_temp.filePointer){
		closeFile(file_userdb);
		return;
	}


	//estrutura para meter as informações do ficheiro
	struct userData tempData;

	while(fscanf(file_userdb.filePointer, "%[^;];%[^;];%[^;];%[^;];%[^;];%f;%[^;];%s\n", tempData.name, tempData.username, 
			tempData.password, tempData.email, tempData.contact, &tempData.balance, tempData.perms, tempData.logged) != EOF){

		if ( !strcmp(tempData.username, username)){
			if (newPerms[0]=='1')
				fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;ADMIN;%s\n", tempData.name, tempData.username, tempData.password, 
					tempData.email, tempData.contact, tempData.balance, tempData.logged);
			else if (newPerms[0]=='2')
				fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;USER;%s\n", tempData.name, tempData.username, tempData.password, 
					tempData.email, tempData.contact, tempData.balance, tempData.logged);
		}

		else
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, tempData.password, 
				tempData.email, tempData.contact, tempData.balance, tempData.perms, tempData.logged);
	}


	rename("temp", "userdb");
	closeFile(file_userdb);
	closeFile(file_temp);
	cleanScreen();
	fprintf(stdout, "O nome do utilizador com o username \"%s\" foi alterado com sucesso!\n\n\n", username);
}

void deleteUser(){

	char username[USERNAME_MAX_SIZE+2];

	fputs("-----Eliminar um Utilizador-----\n\n", stdout);

	fputs("Qual é o username do utilizador que deseja eliminar? ", stdout);
	getString(username, USERNAME_MAX_SIZE+2);


	struct fileStructure file_userdb = openFile("userdb", "READ");

	if (!file_userdb.filePointer)
		return;

	char fileName[NAME_MAX_SIZE+1], fileUsername[USERNAME_MAX_SIZE+1];
	bool found=false;

	while (fscanf(file_userdb.filePointer, "%[^;];%[^;];%*[^;];%*[^;];%*[^;];%*f;%*[^;];%*s\n", fileName, fileUsername)!=EOF){
		if ( !strcmp(fileUsername, username)) {
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_userdb);
		cleanScreen();
		fprintf(stdout, "Erro! O utilizador com o username \"%s\" não foi encontrado.\n\n\n", username);
		return;
	}

	closeFile(file_userdb);


	char confirmation[CONFIRMATION_MAX_SIZE+2];

	fprintf(stdout, "\nTem mesmo a certeza que pretende eliminar o utilizador \"%s\" com o nome \"%s\"? (SIM/NAO): ", fileUsername, fileName);
	getString(confirmation, CONFIRMATION_MAX_SIZE+2);

	if (!checkConfirmation(confirmation)){
		cleanScreen();
		fputs("Ação cancelada!\n\n\n", stdout);
		return;
	}


	file_userdb = openFile("userdb", "WRITE");

	if (!file_userdb.filePointer)
		return;

	struct fileStructure file_temp = openFile("temp", "APPEND");

	if (!file_temp.filePointer){
		closeFile(file_userdb);
		return;
	}


	//estrutura para meter as informações do ficheiro
	struct userData tempData;

	while(fscanf(file_userdb.filePointer, "%[^;];%[^;];%[^;];%[^;];%[^;];%f;%[^;];%s\n", tempData.name, tempData.username, 
			tempData.password, tempData.email, tempData.contact, &tempData.balance, tempData.perms, tempData.logged) != EOF){

		if ( !strcmp(tempData.username, username))
			continue;

		else
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, tempData.password, 
				tempData.email, tempData.contact, tempData.balance, tempData.perms, tempData.logged);
	}


	rename("temp", "userdb");
	closeFile(file_userdb);
	closeFile(file_temp);
	cleanScreen();


	//variaveis para concatenar
	char createDir[FILE_MAX_SIZE]="usersdb/";      //8 (usersdb/) + 20 (USERNAME_MAX_SIZE) + 1 ('\0')
	strcat(createDir, username);

	char createShoplist[FILE_MAX_SIZE]="usersdb/";      //28 = diretório + 8 (shoplist) + 1 ('\0')
	strcat(createShoplist, username);
	strcat(createShoplist, "/");
	strcat(createShoplist, "shoplist");

	remove(createShoplist);
	remove(createDir);
	
	cleanScreen();
	fprintf(stdout, "O utilizador com o username \"%s\" foi eliminado com sucesso!\n\n\n", username);
	if ( !strcmp(username, loggedUsername)){
		fputs("Eliminou o seu próprio utilizador. O programa irá agora terminar.\n", stdout);
		fputs("Saindo do programa...\n", stdout);
		exit(0);
	}
}

void menuManageProduct(){

	char option[OPTION_MAX_SIZE+2]="0";

	while(option[0]!='5'){
		fputs("-----Gestão do Stock-----\n\n", stdout);
		fputs("1) Pesquisar produtos\n", stdout);
		fputs("2) Adicionar um produto\n", stdout);
		fputs("3) Modificar um produto\n", stdout);
		fputs("4) Eliminar um produto\n", stdout);
		fputs("5) Voltar ao menu principal\n", stdout);
		fputs("\nEscolha: ", stdout);
		getString(option, OPTION_MAX_SIZE+2);

		if ( !checkNewOption(option))
			option[0]='a';

		switch(option[0]){
			case '1': cleanScreen();
				searchProduct();
				break;
			case '2': cleanScreen();
				addProduct();
				break;
			case '3': cleanScreen();
				menuChangeProduct();
				break;
			case '4': cleanScreen();
				deleteProduct();
				break;
			case '5': cleanScreen();
				break;
			default: cleanScreen();
				fputs("Erro! Opção não reconhecida.\n\n\n", stdout);
				break;
		}
	}
}

void searchProduct(){
	char name[NAME_MAX_SIZE+2];

	fputs("-----Pesquisar Produtos-----\n\n\n", stdout);
	fputs("Padrão dos nomes dos produtos a pesquisar: (ENTER para listar todos os produtos) ", stdout);
	getString(name, NAME_MAX_SIZE+2);

	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;


	cleanScreen();      //limpar o ecra para ficar bonito

	char fileCode[CODE_SIZE+1], fileName[NAME_MAX_SIZE+1];
	int quantity;
	float price;

	int i, j;
	bool pattern, found_at_least_one=false;
	while (fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", fileCode, fileName,
		&quantity, &price)!=EOF){
		
		for (i=0; fileName[i]!='\0'; i++){
			if (fileName[i]==name[0]){      /*se a letra que estiver no index i da palavra fileName 
													for igual à primeira letra da palavra name*/
				pattern = true;
				for (j=1; fileName[i]!='\0' && name[j]!='\0'; j++){      /*como alguma letra do fileName é igual à 
																				primeira letra do name que queremos pesquisar, 
																				entrar noutro ciclo para percorrer o fileName 
																				e o name pesquisa ao mesmo tempo*/
					if (fileName[i+j]!=name[j]){
						pattern=false;
						break;
					}
				}
			}
			else if (name[0]=='\0'){
				pattern=true;
				i=0;      //i=0 e j=0 para não dar Segmentation Fault no teste de imprimir as coisas quando o name[0]=='\0'
				j=0;
			}
			else if (name[0]!='\0')
				pattern=false;

			if (pattern){
				if ((fileName[i+j]=='\0' && name[j]=='\0') || (fileName[i+j]!='\0' && name[j]=='\0') || (name[0]=='\0')){
					found_at_least_one=true;
					fprintf(stdout, "Código: %s\n", fileCode);
					fprintf(stdout, "Nome: %s\n", fileName);
					fprintf(stdout, "Quantidade disponível: %d\n", quantity);
					fprintf(stdout, "Preço por unidade: %.2f €\n\n\n", price);
					break;
				}
			}
		}
	}

	if ( !found_at_least_one)
		fprintf(stdout, "Não foi encontrado nenhum produto que tivesse \"%s\" contido no nome.\n\n\n", name);

	closeFile(file_productdb);
}

void addProduct(){

	char code[CODE_SIZE+2], name[NAME_MAX_SIZE+2], stringQuantity[QUANTITY_MAX_SIZE_STRING+2], stringPrice[MONEY_MAX_SIZE_STRING+2];

	fputs("-----Adicionar um produto-----\n\n", stdout);
	fputs("* = obrigatório ser original\n", stdout);
	fputs("Código (9 algarismos): (ENTER para código aleatório)* ", stdout);
	getString(code, CODE_SIZE+2);

	//verificar se todos os campos são válidos, senão sair sem escrever para o ficheiro
	if (!checkNewCode(code)){
		cleanScreen();
		fputs("Erro! Código não válido.\n\n\n", stdout);
		return;
	}

	fputs("Nome (não pode conter ';' e tem que ter entre 6 a 50 caracteres)*: ", stdout);
	getString(name, NAME_MAX_SIZE+2);
	if (!checkNewName(name)){      //usar a mesma função dos utilizadores porque as propriedades são iguais
		cleanScreen();
		fputs("Erro! Nome não válido.\n\n\n", stdout);
		return;
	}

	fputs("Quantidade (entre 0 e 2147483647 e tem que ser número inteiro): ", stdout);      //2147483647 = max int
	getString(stringQuantity, QUANTITY_MAX_SIZE_STRING+2);
	if (!checkNewQuantity(stringQuantity)){
		cleanScreen();
		fputs("Erro! Quantidade não válida.\n\n\n", stdout);
		return;
	}
	int quantity = stringToInt(stringQuantity);

	fputs("Preço: (entre 0.0 e 340282346638528859811704183484516925440.0, até 2 casas decimais e usar '.' em vez de ','): ", stdout);
	getString(stringPrice, MONEY_MAX_SIZE_STRING+2);
	if (!checkNewBalance(stringPrice)){      //usar a mesma função do saldo da conta bancária dos utilizadores porque as propriedades são iguais
		cleanScreen();
		fputs("Erro! Preço não válido.\n\n\n", stdout);
		return;
	}
	float price = stringToFloat(stringPrice);



	struct fileStructure file_productdb = openFile("productdb", "APPEND");

	if (!file_productdb.filePointer)
		return;


	char fileCode[CODE_SIZE+1], fileName[NAME_MAX_SIZE+1];
	//ver se já existe algum produto com o mesmo código ou nome
	while (fscanf(file_productdb.filePointer, "%[^;];%[^;];%*d;%*f\n", fileCode, fileName)!=EOF){
		if ( !strcmp(fileCode, code)) {
			closeFile(file_productdb);
			cleanScreen();
			fprintf(stdout, "Erro! O produto com o código \"%s\" já existe.\n\n\n", code);
			return;
		}
		else if ( !strcmp(fileName, name)) {
			closeFile(file_productdb);
			cleanScreen();
			fprintf(stdout, "Erro! O produto com o nome \"%s\" já existe.\n\n\n", name);
			return;
		}
	}


	/*este ciclo a seguir definido, se encontrar um código igual ao que queremos dar,
	mete que não é diferente e sai do ciclo mais próximo, gerando outra vez outro código aleatoriamente
	e verifica outra vez de é diferente ou não, repetindo-se as vezes que forem necessárias
	Não é preciso verificar se o código está certo porque o modo como é gerado está correto*/
	if (code[0]=='\0'){      //se o código for vazio então gerar um aleatório
		bool different=false;
		while (!different){
			generateCode(code);
			fseek(file_productdb.filePointer, 0, SEEK_SET);      //andar com o pointer do ficheiro para o início do ficheiro
			different=true;
			while (fscanf(file_productdb.filePointer, "%[^;];%*[^;];%*d;%*f\n", fileCode)!=EOF){
				if ( !strcmp(fileCode, code)) {      //percorrer o ficheiro e se encontrar um código igual, meter que não é diferente e sair do ciclo mais próximo
					different=false;
					break;
				}
			}
		}
	}


	//escrever para o ficheiro productdb
	fprintf(file_productdb.filePointer, "%s;%s;%d;%f\n", code, name, quantity, price);


	closeFile(file_productdb);
	cleanScreen();      //limpar o ecra
	fprintf(stdout, "O produto \"%s\", com o nome \"%s\" foi criado com sucesso!\n\n\n", code, name);
}

void menuChangeProduct(){

	char option[OPTION_MAX_SIZE+2]="0";

	while(option[0]!='5'){
		fputs("-----Modificar Stock-----\n\n", stdout);
		fputs("1) Mudar o código de um produto\n", stdout);
		fputs("2) Mudar o nome de um produto\n", stdout);
		fputs("3) Mudar a quantidade disponível de um produto\n", stdout);
		fputs("4) Mudar o preço de um produto\n", stdout);
		fputs("5) Voltar ao menu de gestão do stock\n", stdout);
		fputs("\nEscolha: ", stdout);
		getString(option, OPTION_MAX_SIZE+2);

		if ( !checkNewOption(option))
			option[0]='a';

		switch(option[0]){
			case '1': cleanScreen();
				changeProductCode();
				break;
			case '2': cleanScreen();
				changeProductName();
				break;
			case '3': cleanScreen();
				changeProductAvailableQuantity();
				break;
			case '4': cleanScreen();
				changeProductPrice();
				break;
			case '5': cleanScreen();
				break;
			default: cleanScreen();
				fputs("Erro! Opção não reconhecida.\n\n\n", stdout);
				break;
		}
	}
}

void changeProductCode(){

	char code[CODE_SIZE+2];

	fputs("-----Mudar código de um produto-----\n\n", stdout);

	fputs("Qual é o código do produto que deseja modificar? ", stdout);
	getString(code, CODE_SIZE+2);


	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;

	char fileCode[CODE_SIZE+1], fileName[NAME_MAX_SIZE+1];
	bool found=false;

	while(fscanf(file_productdb.filePointer, "%[^;];%[^;];%*d;%*f", fileCode, fileName) != EOF){
		if ( !strcmp(fileCode, code)){
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_productdb);
		cleanScreen();
		fprintf(stdout, "Erro! O produto com o código \"%s\" não foi encontrado.\n\n\n", code);
		return;
	}

	closeFile(file_productdb);


	char newCode[CODE_SIZE+2];

	fputs("\n\n\n* = obrigatório ser original\n\n", stdout);
	fprintf(stdout, "Nome do produto: %s\n\n", fileName);
	fputs("Novo código (9 algarismos): (ENTER para código aleatório)*: ", stdout);
	getString(newCode, CODE_SIZE+2);

	if (!checkNewCode(newCode)){
		cleanScreen();
		fputs("Erro! Novo código não válido.\n\n\n", stdout);
		return;
	}


	file_productdb = openFile("productdb", "WRITE");

	if (!file_productdb.filePointer)
		return;

	/*este ciclo a seguir definido, se encontrar um código igual ao que queremos dar,
	mete que não é diferente e sai do ciclo mais próximo, gerando outra vez outro código aleatoriamente
	e verifica outra vez de é diferente ou não, repetindo-se as vezes que forem necessárias
	Não é preciso verificar se o código está certo porque o modo como é gerado está correto*/
	if (newCode[0]=='\0'){      //se o código for vazio então gerar um aleatório
		bool different=false;
		while (!different){
			generateCode(newCode);
			fseek(file_productdb.filePointer, 0, SEEK_SET);      //andar com o pointer do ficheiro para o início do ficheiro
			while (fscanf(file_productdb.filePointer, "%[^;];%*[^;];%*d;%*f\n", fileCode)!=EOF){
				different=true;
				if ( !strcmp(fileCode, newCode)) {      //percorrer o ficheiro e se encontrar um código igual, meter que não é diferente e sair do ciclo mais próximo
					different=false;
					break;
				}
			}
		}
	}
	else {
		found=false;
		while(fscanf(file_productdb.filePointer, "%[^;];%[^;];%*d;%*f", fileCode, fileName) != EOF){
			if ( !strcmp(fileCode, newCode)){
				found=true;
				break;
			}
		}

		if (found){
			closeFile(file_productdb);
			cleanScreen();
			fprintf(stdout, "Erro! O produto com o código \"%s\" já existe.\n\n\n", newCode);
			return;
		}
	}


	struct fileStructure file_temp = openFile("temp", "APPEND");

	if (!file_temp.filePointer){
		closeFile(file_productdb);
		return;
	}


	//estrutura para meter as informações do ficheiro
	struct productData tempData;

	fseek(file_productdb.filePointer, 0, SEEK_SET);
	while(fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", tempData.code, tempData.name, 
		&tempData.quantity, &tempData.price) != EOF){

		if ( !strcmp(tempData.code, code))
			fprintf(file_temp.filePointer, "%s;%s;%d;%f\n", newCode, tempData.name, 
				tempData.quantity, tempData.price);

		else
			fprintf(file_temp.filePointer, "%s;%s;%d;%f\n", tempData.code, tempData.name, 
				tempData.quantity, tempData.price);
	}


	rename("temp", "productdb");
	closeFile(file_productdb);
	closeFile(file_temp);
	cleanScreen();
	fprintf(stdout, "O código do produto com o código \"%s\" foi alterado com sucesso!\n\n\n", code);
}

void changeProductName(){

	char code[CODE_SIZE+2];

	fputs("-----Mudar nome de um produto-----\n\n", stdout);

	fputs("Qual é o código do produto que deseja modificar? ", stdout);
	getString(code, CODE_SIZE+2);


	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;

	char fileCode[CODE_SIZE+1], fileName[NAME_MAX_SIZE+1];
	bool found=false;

	while(fscanf(file_productdb.filePointer, "%[^;];%[^;];%*d;%*f", fileCode, fileName) != EOF){
		if ( !strcmp(fileCode, code)){
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_productdb);
		cleanScreen();
		fprintf(stdout, "Erro! O produto com o código \"%s\" não foi encontrado.\n\n\n", code);
		return;
	}

	closeFile(file_productdb);


	char newName[NAME_MAX_SIZE+2];

	fprintf(stdout, "\n\n\nNome atual do produto \"%s\": %s\n\n", code, fileName);
	fputs("* = obrigatório ser original\n\n", stdout);
	fputs("Novo nome (não pode conter ';' e tem que ter entre 6 a 50 caracteres)*: ", stdout);
	getString(newName, NAME_MAX_SIZE+2);

	if (!checkNewName(newName)){
		cleanScreen();
		fputs("Erro! Novo nome não válido.\n\n\n", stdout);
		return;
	}



	file_productdb = openFile("productdb", "WRITE");

	if (!file_productdb.filePointer)
		return;


	found=false;
	while(fscanf(file_productdb.filePointer, "%[^;];%[^;];%*d;%*f", fileCode, fileName) != EOF){
		if ( !strcmp(fileName, newName)){
			found=true;
			break;
		}
	}

	if (found){
		closeFile(file_productdb);
		cleanScreen();
		fprintf(stdout, "Erro! O produto com o nome \"%s\" já existe.\n\n\n", newName);
		return;
	}


	struct fileStructure file_temp = openFile("temp", "APPEND");

	if (!file_temp.filePointer){
		closeFile(file_productdb);
		return;
	}


	//estrutura para meter as informações do ficheiro
	struct productData tempData;

	fseek(file_productdb.filePointer, 0, SEEK_SET);
	while(fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", tempData.code, tempData.name, 
		&tempData.quantity, &tempData.price) != EOF){

		if ( !strcmp(tempData.code, code))
			fprintf(file_temp.filePointer, "%s;%s;%d;%f\n", tempData.code, newName, 
				tempData.quantity, tempData.price);

		else
			fprintf(file_temp.filePointer, "%s;%s;%d;%f\n", tempData.code, tempData.name, 
				tempData.quantity, tempData.price);
	}


	rename("temp", "productdb");
	closeFile(file_productdb);
	closeFile(file_temp);
	cleanScreen();
	fprintf(stdout, "O nome do produto com o código \"%s\" foi alterado com sucesso!\n\n\n", code);
}

void changeProductAvailableQuantity(){

	char code[CODE_SIZE+2];

	fputs("-----Mudar a quantidade disponível de um produto-----\n\n", stdout);

	fputs("Qual é o código do produto que deseja modificar? ", stdout);
	getString(code, CODE_SIZE+2);


	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;

	char fileCode[CODE_SIZE+1];
	int fileAvailableQuantity;
	bool found=false;

	while(fscanf(file_productdb.filePointer, "%[^;];%*[^;];%d;%*f", fileCode, &fileAvailableQuantity) != EOF){
		if ( !strcmp(fileCode, code)){
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_productdb);
		cleanScreen();
		fprintf(stdout, "Erro! O produto com o código \"%s\" não foi encontrado.\n\n\n", code);
		return;
	}

	closeFile(file_productdb);


	char newStringQuantity[QUANTITY_MAX_SIZE_STRING+2];

	fprintf(stdout, "\n\n\nQuantidade atual do produto \"%s\": %d\n\n", code, fileAvailableQuantity);
	fputs("Nova quantidade (entre 0 e 2147483647 e tem que ser número inteiro): ", stdout);
	getString(newStringQuantity, QUANTITY_MAX_SIZE_STRING+2);

	if (!checkNewQuantity(newStringQuantity)){
		cleanScreen();
		fputs("Erro! Nova quantidade não válida.\n\n\n", stdout);
		return;
	}


	//transformar a string com a nova quantidade para um int
	int newQuantity = stringToInt(newStringQuantity);


	file_productdb = openFile("productdb", "WRITE");

	if (!file_productdb.filePointer)
		return;

	struct fileStructure file_temp = openFile("temp", "APPEND");

	if (!file_temp.filePointer){
		closeFile(file_productdb);
		return;
	}


	//estrutura para meter as informações do ficheiro
	struct productData tempData;

	while(fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", tempData.code, tempData.name, 
		&tempData.quantity, &tempData.price) != EOF){

		if ( !strcmp(tempData.code, code))
			fprintf(file_temp.filePointer, "%s;%s;%d;%f\n", tempData.code, tempData.name, 
				newQuantity, tempData.price);

		else
			fprintf(file_temp.filePointer, "%s;%s;%d;%f\n", tempData.code, tempData.name, 
				tempData.quantity, tempData.price);
	}


	rename("temp", "productdb");
	closeFile(file_productdb);
	closeFile(file_temp);
	cleanScreen();
	fprintf(stdout, "A quantidade do produto com o código \"%s\" foi alterada com sucesso!\n\n\n", code);
}

void changeProductPrice(){

	char code[CODE_SIZE+2];

	fputs("-----Mudar o preço de um produto-----\n\n", stdout);

	fputs("Qual é o código do produto que deseja modificar? ", stdout);
	getString(code, CODE_SIZE+2);


	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;

	char fileCode[CODE_SIZE+1];
	float filePrice;
	bool found=false;

	while(fscanf(file_productdb.filePointer, "%[^;];%*[^;];%*d;%f", fileCode, &filePrice) != EOF){
		if ( !strcmp(fileCode, code)){
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_productdb);
		cleanScreen();
		fprintf(stdout, "Erro! O produto com o código \"%s\" não foi encontrado.\n\n\n", code);
		return;
	}

	closeFile(file_productdb);


	char newStringPrice[MONEY_MAX_SIZE_STRING+2];

	fprintf(stdout, "\n\n\nPreço atual do produto \"%s\": %.2f €\n\n", code, filePrice);
	fputs("Novo preço (entre 0.0 e 340282346638528859811704183484516925440.0, até 2 casas decimais e usar '.' em vez de ','): ", stdout);
	getString(newStringPrice, MONEY_MAX_SIZE_STRING+2);

	if (!checkNewBalance(newStringPrice)){      //usar a mesma função do saldo da conta bancária dos utilizadores porque as propriedades são iguais
		cleanScreen();
		fputs("Erro! Novo preço não válido.\n\n\n", stdout);
		return;
	}


	float newPrice = stringToFloat(newStringPrice);


	file_productdb = openFile("productdb", "WRITE");

	if (!file_productdb.filePointer)
		return;

	struct fileStructure file_temp = openFile("temp", "APPEND");

	if (!file_temp.filePointer){
		closeFile(file_productdb);
		return;
	}


	//estrutura para meter as informações do ficheiro
	struct productData tempData;

	while(fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", tempData.code, tempData.name, 
		&tempData.quantity, &tempData.price) != EOF){

		if ( !strcmp(tempData.code, code))
			fprintf(file_temp.filePointer, "%s;%s;%d;%f\n", tempData.code, tempData.name, 
				tempData.quantity, newPrice);

		else
			fprintf(file_temp.filePointer, "%s;%s;%d;%f\n", tempData.code, tempData.name, 
				tempData.quantity, tempData.price);
	}


	rename("temp", "productdb");
	closeFile(file_productdb);
	closeFile(file_temp);
	cleanScreen();
	fprintf(stdout, "O preço do produto com o código \"%s\" foi alterado com sucesso!\n\n\n", code);
}

void deleteProduct(){

	char code[CODE_SIZE+2];

	fputs("-----Eliminar um produto-----\n\n", stdout);

	fputs("Qual é o código do produto que deseja eliminar? ", stdout);
	getString(code, CODE_SIZE+2);


	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;


	char fileCode[CODE_SIZE+1], fileName[NAME_MAX_SIZE+1];
	bool found=false;

	while (fscanf(file_productdb.filePointer, "%[^;];%[^;];%*d;%*f\n", fileCode, fileName)!=EOF){
		if ( !strcmp(code, fileCode)){
			found=true;
			break;
		}
	}

	if (!found){
		closeFile(file_productdb);
		cleanScreen();
		fprintf(stdout, "Erro! O produto com o código \"%s\" não foi encontrado.\n\n\n", code);
		return;
	}

	closeFile(file_productdb);


	char confirmation[CONFIRMATION_MAX_SIZE+2];

	fprintf(stdout, "\nTem mesmo a certeza que pretende eliminar o produto \"%s\" com o nome \"%s\"? (SIM/NAO): ", fileCode, fileName);
	getString(confirmation, CONFIRMATION_MAX_SIZE+2);

	if (!checkConfirmation(confirmation)){
		cleanScreen();
		fputs("Ação cancelada!\n\n\n", stdout);
		return;
	}


	file_productdb = openFile("productdb", "WRITE");

	if (!file_productdb.filePointer)
		return;

	struct fileStructure file_temp = openFile("temp", "APPEND");

	if (!file_temp.filePointer){
		closeFile(file_productdb);
		return;
	}


	//estrutura para meter as informações do ficheiro
	struct productData tempData;

	while(fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", tempData.code, tempData.name, 
		&tempData.quantity, &tempData.price) != EOF){

		if ( !strcmp(tempData.code, code))
			continue;

		else
			fprintf(file_temp.filePointer, "%s;%s;%d;%f\n", tempData.code, tempData.name, 
				tempData.quantity, tempData.price);
	}


	rename("temp", "productdb");
	closeFile(file_productdb);
	closeFile(file_temp);
	cleanScreen();
	fprintf(stdout, "O produto com o código \"%s\" foi eliminado com sucesso!\n\n\n", code);
}

void menuStats(){

	char option[OPTION_MAX_SIZE+2]="0";

	while(option[0]!='8'){
		fputs("-----Menu de Estatísticas de Vendas-----\n\n", stdout);
		fputs("1) Estatísticas de um determinado produto\n", stdout);
		fputs("2) TOP de produtos comprados por um determinado utilizador\n", stdout);
		fputs("3) TOP de produtos vendidos\n", stdout);
		fputs("4) Estatísticas diárias\n", stdout);
		fputs("5) TOP de produtos vendidos num dia\n", stdout);
		fputs("6) Estatísticas mensais\n", stdout);
		fputs("7) TOP de produtos vendidos num mes\n", stdout);
		fputs("8) Voltar ao menu principal\n", stdout);
		fputs("\nEscolha: ", stdout);
		getString(option, OPTION_MAX_SIZE+2);

		if ( !checkNewOption(option))
			option[0]='a';      /*como todas as opções sao algarismos, isto vai fazer 
								com que a opção não seja reconhecida*/

		switch(option[0]){
			case '1': cleanScreen();
				productStats();
				break;
			case '2': cleanScreen();
				topProductUserStats();
				break;
			case '3': cleanScreen();
				topProductStats();
				break;
			case '4': cleanScreen();
				dailyStats();
				break;
			case '5': cleanScreen();
				topDailyStats();
				break;
			case '6': cleanScreen();
				monthlyStats();
				break;
			case '7': cleanScreen();
				topMonthlyStats();
				break;
			case '8': cleanScreen();
				break;
			default: cleanScreen();
				fputs("Erro! Opção não reconhecida.\n\n\n", stdout);
				break;
		}
	}
}

void productStats(){
	char code[CODE_SIZE+2];

	fputs("-----Estatísticas de Vendas de um Produto-----\n\n\n", stdout);
	fputs("Código do produto a processar estatísticas: ", stdout);
	getString(code, CODE_SIZE+2);


	//ver se o produto existe no stock
	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;

	char fileCode[CODE_SIZE+1];
	float filePrice;
	bool found=false;

	while (fscanf(file_productdb.filePointer, "%[^;];%*[^;];%*d;%f\n", fileCode, &filePrice)!=EOF){
		if ( !strcmp(fileCode, code)) {
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_productdb);
		cleanScreen();
		fprintf(stdout, "Erro! O produto com o código \"%s\" não foi encontrado.\n\n\n", code);
		return;
	}

	closeFile(file_productdb);


	struct fileStructure file_salesdb = openFile("salesdb", "READ");

	if (!file_salesdb.filePointer)
		return;


	//meter as informações para uma estrutura
	struct salesData tempData;
	long int nr_of_unit_sold = 0;

	while (fscanf(file_salesdb.filePointer, "%[^;];%d;%*[^;];%*[^/]/%*[^/]/%*s\n", tempData.code, 
		&tempData.quantity)!=EOF){
		
		if (!strcmp(code, tempData.code))
			nr_of_unit_sold += tempData.quantity;
	}

	closeFile(file_salesdb);


	cleanScreen();      //limpar o ecra para ficar bonito
	fprintf(stdout, "Número de unidades vendidas do produto com o código \"%s\": %ld\n", code, nr_of_unit_sold);
	fprintf(stdout, "Lucro: %.2lf €\n\n\n", filePrice*(float)nr_of_unit_sold);
}

void topProductUserStats(){
	char username[USERNAME_MAX_SIZE+1];

	fputs("-----Estatísticas de Vendas de Produtos Comprados por um Determinado Utilizador (TOP)-----\n\n\n", stdout);
	fputs("Username do utilizador a processar estatísticas: ", stdout);
	getString(username, USERNAME_MAX_SIZE+2);


	struct fileStructure file_userdb = openFile("userdb", "READ");

	if (!file_userdb.filePointer)
		return;


	bool found=false;
	char fileUsername[USERNAME_MAX_SIZE+1];

	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%*[^;];%*[^;];%*[^;];%*f;%*[^;];%*s\n", fileUsername)!=EOF){
		if ( !strcmp(fileUsername, username)) {
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_userdb);
		cleanScreen();
		fprintf(stdout, "Erro! O utilizador com o username \"%s\" não foi encontrado.\n\n\n", username);
		return;
	}

	closeFile(file_userdb);



	struct fileStructure file_salesdb = openFile("salesdb", "READ");

	if (!file_salesdb.filePointer)
		return;


	int linhas=0;      //número máximo de posições ocupadas (número máximo de produtos diferentes comprados por aquele utilizador)
	while (fscanf(file_salesdb.filePointer, "%*[^;];%*d;%[^;];%*[^/]/%*[^/]/%*s\n", fileUsername)!=EOF){
		if ( !strcmp(fileUsername, username))
			linhas++;
	}

	if ( !linhas){
		closeFile(file_salesdb);
		cleanScreen();
		fprintf(stdout, "Erro! O utilizador com o username \"%s\" ainda não comprou nada.\n\n\n", username);
		return;
	}


	struct salesData tempData[linhas];

	//voltar com o pointer para o início do ficheiro para usar no fscanf
	fseek(file_salesdb.filePointer, 0, SEEK_SET);

	int linhas2=0, i;      //linhas2 = número de produtos diferentes vendidos naquele dia
	found=false;
	char fileCode[CODE_SIZE+1];
	int fileQuantity;
	//meter as informações do ficheiro dentro do array de estruturas
	while (fscanf(file_salesdb.filePointer, "%[^;];%d;%[^;];%*[^/]/%*[^/]/%*s\n", fileCode, &fileQuantity, fileUsername)!=EOF){

		if ( !strcmp(fileUsername, username)){
			found=false;
			for (i=0; i<linhas2; i++){
				if (!strcmp(fileCode, tempData[i].code)){
					found=true;
					break;
				}
			}
			if (found)
				tempData[i].quantity+=fileQuantity;
			else {
				strcpy(tempData[linhas2].code, fileCode);
				tempData[linhas2].quantity = fileQuantity;
				linhas2++;
			}
		}
	}

	//se metermos closeFile(file_salesdb); dá segfault
	file_salesdb.lock->l_type = F_UNLCK;

	fcntl(file_salesdb.fd, F_SETLKW, file_salesdb.lock);

	//se o ponteiro for nulo, não fazer isto ou dá falha de segmentação
	if (file_salesdb.filePointer)
		fclose(file_salesdb.filePointer);

	close(file_salesdb.fd);

	//ordenar por quantidades vendidas
	struct salesData tempData_2;
	for (i=0; i<linhas2; i++){
		for (int j=i; j<linhas2; j++){
			if (tempData[j].quantity > tempData[i].quantity){
				tempData_2=tempData[i];
				tempData[i]=tempData[j];
				tempData[j]=tempData_2;
			}
		}
	}


	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;
	
	char fileName[NAME_MAX_SIZE+1];
	float filePrice;
	found=false;

	cleanScreen();

	for (i=0; i<linhas2; i++){

		fprintf(stdout, "#%d\n", i+1);
		fprintf(stdout, "Código: %s\n", tempData[i].code);      //imprimir código
		fseek(file_productdb.filePointer, 0, SEEK_SET);      //andar com o ponteiro do ficheiro para o inicio do ficheiro
		found=false;

		while(fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", fileCode, fileName, 
			&fileQuantity, &filePrice)!=EOF){      //ir buscar informações do produto à base de dados de produtos

			if (!strcmp(tempData[i].code, fileCode)){
				fprintf(stdout, "Nome: %s\n", fileName);
				fprintf(stdout, "Quantidade disponível: %d\n", fileQuantity);
				fprintf(stdout, "Preço por unidade: %.2f €\n", filePrice);
				found=true;
				break;
			}
		}
		if (!found)
			fputs("Produto não existente na base de dados.\n", stdout);

		fprintf(stdout, "Unidades vendidas: %d\n", tempData[i].quantity);

		if (found)
			fprintf(stdout, "Lucro: %.2lf €\n\n\n", filePrice*(float)tempData[i].quantity);
	}


	closeFile(file_productdb);

}

void topProductStats(){

	fputs("-----Estatísticas de Vendas de Produtos (TOP)-----\n\n\n", stdout);


	struct fileStructure file_salesdb = openFile("salesdb", "READ");

	if (!file_salesdb.filePointer)
		return;


	int linhas=0;      //número máximo de posições ocupadas (número máximo de produtos diferentes vendidos na data referida)
	while (fscanf(file_salesdb.filePointer, "%*[^;];%*d;%*[^;];%*[^/]/%*[^/]/%*s\n")!=EOF)
			linhas++;

	if ( !linhas){
		closeFile(file_salesdb);
		fputs("Erro! O registo de vendas está vazio\n\n\n", stdout);
		return;
	}


	struct salesData tempData[linhas];

	//voltar com o pointer para o início do ficheiro para usar no fscanf
	fseek(file_salesdb.filePointer, 0, SEEK_SET);

	int linhas2=0, i;      //linhas2 = número de produtos diferentes vendidos naquele dia
	bool found=false;
	char fileCode[CODE_SIZE+1];
	int fileQuantity;
	//meter as informações do ficheiro dentro do array de estruturas
	while (fscanf(file_salesdb.filePointer, "%[^;];%d;%*[^;];%*[^/]/%*[^/]/%*s\n", fileCode, &fileQuantity)!=EOF){

		found=false;
		for (i=0; i<linhas2; i++){
			if (!strcmp(fileCode, tempData[i].code)){
				found=true;
				break;
			}
		}
		if (found)
			tempData[i].quantity+=fileQuantity;
		else {
			strcpy(tempData[linhas2].code, fileCode);
			tempData[linhas2].quantity = fileQuantity;
			linhas2++;
		}
	}

	//se metermos closeFile(file_salesdb); dá segfault
	file_salesdb.lock->l_type = F_UNLCK;

	fcntl(file_salesdb.fd, F_SETLKW, file_salesdb.lock);

	//se o ponteiro for nulo, não fazer isto ou dá falha de segmentação
	if (file_salesdb.filePointer)
		fclose(file_salesdb.filePointer);

	close(file_salesdb.fd);

	//ordenar por quantidades vendidas
	struct salesData tempData_2;
	for (i=0; i<linhas2; i++){
		for (int j=i; j<linhas2; j++){
			if (tempData[j].quantity > tempData[i].quantity){
				tempData_2=tempData[i];
				tempData[i]=tempData[j];
				tempData[j]=tempData_2;
			}
		}
	}


	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;
	
	char fileName[NAME_MAX_SIZE+1];
	float filePrice;
	found=false;

	cleanScreen();

	for (i=0; i<linhas2; i++){

		fprintf(stdout, "#%d\n", i+1);
		fprintf(stdout, "Código: %s\n", tempData[i].code);      //imprimir código
		fseek(file_productdb.filePointer, 0, SEEK_SET);      //andar com o ponteiro do ficheiro para o inicio do ficheiro
		found=false;

		while(fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", fileCode, fileName, 
			&fileQuantity, &filePrice)!=EOF){      //ir buscar informações do produto à base de dados de produtos

			if (!strcmp(tempData[i].code, fileCode)){
				fprintf(stdout, "Nome: %s\n", fileName);
				fprintf(stdout, "Quantidade disponível: %d\n", fileQuantity);
				fprintf(stdout, "Preço por unidade: %.2f €\n", filePrice);
				found=true;
				break;
			}
		}
		if (!found)
			fputs("Produto não existente na base de dados.\n", stdout);

		fprintf(stdout, "Unidades vendidas: %d\n", tempData[i].quantity);

		if (found)
			fprintf(stdout, "Lucro: %.2lf €\n\n\n", filePrice*(float)tempData[i].quantity);
	}


	closeFile(file_productdb);

}

void dailyStats(){
	char code[CODE_SIZE+2], day[DAY_MAX_SIZE+2], month[MONTH_MAX_SIZE+2], year[YEAR_MAX_SIZE+2];

	fputs("-----Estatísticas diárias-----\n\n\n", stdout);
	fputs("Código do produto a processar estatísticas: ", stdout);
	getString(code, CODE_SIZE+2);


	//ver se o produto existe no stock
	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;

	char fileCode[CODE_SIZE+1];
	float filePrice;
	bool found=false;

	while (fscanf(file_productdb.filePointer, "%[^;];%*[^;];%*d;%f\n", fileCode, &filePrice)!=EOF){
		if ( !strcmp(fileCode, code)) {
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_productdb);
		fprintf(stdout, "Erro! O produto com o código \"%s\" não foi encontrado.\n\n\n", code);
		return;
	}

	closeFile(file_productdb);


	fputs("\n\nData para processamento de estatísticas:\n\n", stdout);
	fputs("Dia: ", stdout);
	getString(day, DAY_MAX_SIZE+2);
	if (!checkDay(day)){
		cleanScreen();
		fprintf(stdout, "Erro! Dia não válido.\n\n\n");
		return;
	}

	fputs("Mes: ", stdout);
	getString(month, MONTH_MAX_SIZE+2);
	if (!checkMonth(month)){
		cleanScreen();
		fprintf(stdout, "Erro! Mes não válido.\n\n\n");
		return;
	}

	fputs("Ano: ", stdout);
	getString(year, YEAR_MAX_SIZE+2);      //6 para ex:2017 + verificação de erro + '\0'
	if (!checkYear(year)){
		cleanScreen();
		fprintf(stdout, "Erro! Ano não válido.\n\n\n");
		return;
	}


	struct fileStructure file_salesdb = openFile("salesdb", "READ");

	if (!file_salesdb.filePointer)
		return;


	char fileDay[DAY_MAX_SIZE+1], fileMonth[MONTH_MAX_SIZE+1], fileYear[YEAR_MAX_SIZE+1];
	int fileQuantity;
	long int nr_of_unit_sold = 0;
	while (fscanf(file_salesdb.filePointer, "%[^;];%d;%*[^;];%[^/]/%[^/]/%s\n", fileCode, &fileQuantity, 
		fileDay, fileMonth, fileYear)!=EOF){

		if (!strcmp(code, fileCode) && !strcmp(day, fileDay) && !strcmp(month, fileMonth) && 
			!strcmp(year, fileYear))

			nr_of_unit_sold += fileQuantity;
	}

	closeFile(file_salesdb);

	if ( !nr_of_unit_sold){
		cleanScreen();
		fprintf(stdout, "Erro! O produto com o código \"%s\" não foi encontrado no registo de vendas na data referida.\n\n\n", code);
		return;
	}


	cleanScreen();      //limpar o ecra para ficar bonito
	fprintf(stdout, "Data de estatísticas: %s/%s/%s \n\n", day, month, year);
	fprintf(stdout, "Número de unidades vendidas do produto com o código \"%s\" na data referida: %ld\n", code, nr_of_unit_sold);
	fprintf(stdout, "Lucro: %.2lf €\n\n\n", filePrice*(float)nr_of_unit_sold);
}

void topDailyStats(){
	char day[DAY_MAX_SIZE+2], month[MONTH_MAX_SIZE+2], year[YEAR_MAX_SIZE+2];

	fputs("-----Estatísticas diárias (TOP)-----\n\n\n", stdout);
	fputs("Data para processamento de estatística\n\n", stdout);

	fputs("Dia: ", stdout);
	getString(day, DAY_MAX_SIZE+2);
	if (!checkDay(day)){
		cleanScreen();
		fprintf(stdout, "Erro! Dia não válido.\n\n\n");
		return;
	}

	fputs("Mes: ", stdout);
	getString(month, MONTH_MAX_SIZE+2);
	if (!checkMonth(month)){
		cleanScreen();
		fprintf(stdout, "Erro! Mes não válido.\n\n\n");
		return;
	}

	fputs("Ano: ", stdout);
	getString(year, YEAR_MAX_SIZE+2);      //6 para ex:2017 + verificação de erro + '\0'
	if (!checkYear(year)){
		cleanScreen();
		fprintf(stdout, "Erro! Ano não válido.\n\n\n");
		return;
	}


	struct fileStructure file_salesdb = openFile("salesdb", "READ");

	if (!file_salesdb.filePointer)
		return;


	char fileDay[DAY_MAX_SIZE+1], fileMonth[MONTH_MAX_SIZE+1], fileYear[YEAR_MAX_SIZE+1];
	int linhas=0;      //número máximo de posições ocupadas (número máximo de produtos diferentes vendidos na data referida)
	while (fscanf(file_salesdb.filePointer, "%*[^;];%*d;%*[^;];%[^/]/%[^/]/%s\n", fileDay, fileMonth, fileYear)!=EOF){

		if (!strcmp(day, fileDay) && !strcmp(month, fileMonth) && 
			!strcmp(year, fileYear))

			linhas++;
	}

	if ( !linhas){
		closeFile(file_salesdb);
		cleanScreen();
		fputs("Erro! Não foram encontrados registos de vendas para esse dia.\n\n\n", stdout);
		return;
	}


	struct salesData tempData[linhas];

	//voltar com o pointer para o início do ficheiro para usar no fscanf
	fseek(file_salesdb.filePointer, 0, SEEK_SET);

	int linhas2=0, i;      //linhas2 = número de produtos diferentes vendidos naquele dia
	bool found=false;
	char fileCode[CODE_SIZE+1];
	int fileQuantity;
	//meter as informações do ficheiro dentro do array de estruturas
	while (fscanf(file_salesdb.filePointer, "%[^;];%d;%*[^;];%[^/]/%[^/]/%s\n", fileCode, &fileQuantity, 
		fileDay, fileMonth, fileYear)!=EOF){

		if (!strcmp(fileDay, day) && !strcmp(fileMonth, month) && 
			!strcmp(fileYear, year)) {
			found=false;
			for (i=0; i<linhas2; i++){
				if (!strcmp(fileCode, tempData[i].code)){
					found=true;
					break;
				}
			}
			if (found)
				tempData[i].quantity+=fileQuantity;
			else {
				strcpy(tempData[linhas2].code, fileCode);
				tempData[linhas2].quantity = fileQuantity;
				linhas2++;
			}
		}
	}

	//se metermos closeFile(file_salesdb); dá segfault
	file_salesdb.lock->l_type = F_UNLCK;

	fcntl(file_salesdb.fd, F_SETLKW, file_salesdb.lock);

	//se o ponteiro for nulo, não fazer isto ou dá falha de segmentação
	if (file_salesdb.filePointer)
		fclose(file_salesdb.filePointer);

	close(file_salesdb.fd);

	//ordenar por quantidades vendidas
	struct salesData tempData_2;
	for (i=0; i<linhas2; i++){
		for (int j=i; j<linhas2; j++){
			if (tempData[j].quantity > tempData[i].quantity){
				tempData_2=tempData[i];
				tempData[i]=tempData[j];
				tempData[j]=tempData_2;
			}
		}
	}


	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;
	
	char fileName[NAME_MAX_SIZE+1];
	float filePrice;
	found=false;

	cleanScreen();
	fprintf(stdout, "Data de estatísticas: %s/%s/%s \n\n", day, month, year);

	for (i=0; i<linhas2; i++){

		fprintf(stdout, "#%d\n", i+1);
		fprintf(stdout, "Código: %s\n", tempData[i].code);      //imprimir código
		fseek(file_productdb.filePointer, 0, SEEK_SET);      //andar com o ponteiro do ficheiro para o inicio do ficheiro
		found=false;

		while(fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", fileCode, fileName, 
			&fileQuantity, &filePrice)!=EOF){      //ir buscar informações do produto à base de dados de produtos

			if (!strcmp(tempData[i].code, fileCode)){
				fprintf(stdout, "Nome: %s\n", fileName);
				fprintf(stdout, "Quantidade disponível: %d\n", fileQuantity);
				fprintf(stdout, "Preço por unidade: %.2f €\n", filePrice);
				found=true;
				break;
			}
		}
		if (!found)
			fputs("Produto não existente na base de dados.\n", stdout);

		fprintf(stdout, "Unidades vendidas na data referida: %d\n", tempData[i].quantity);

		if (found)
			fprintf(stdout, "Lucro: %.2lf €\n\n\n", filePrice*(float)tempData[i].quantity);
	}


	closeFile(file_productdb);
}

void monthlyStats(){
	char code[CODE_SIZE+2], month[MONTH_MAX_SIZE+2], year[YEAR_MAX_SIZE+2];

	fputs("-----Estatísticas Mensais-----\n\n\n", stdout);
	fputs("Código do produto a processar estatísticas: ", stdout);
	getString(code, CODE_SIZE+2);


	//ver se o produto existe no stock
	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;

	char fileCode[CODE_SIZE+1];
	float filePrice;
	bool found=false;

	while (fscanf(file_productdb.filePointer, "%[^;];%*[^;];%*d;%f\n", fileCode, &filePrice)!=EOF){
		if ( !strcmp(fileCode, code)) {
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_productdb);
		fprintf(stdout, "Erro! O produto com o código \"%s\" não foi encontrado.\n\n\n", code);
		return;
	}

	closeFile(file_productdb);


	fputs("\n\nData para processamento de estatísticas:\n\n", stdout);
	fputs("Mes: ", stdout);
	getString(month, MONTH_MAX_SIZE+2);
	if (!checkMonth(month)){
		cleanScreen();
		fprintf(stdout, "Erro! Mes não válido.\n\n\n");
		return;
	}

	fputs("Ano: ", stdout);
	getString(year, YEAR_MAX_SIZE+2);      //6 para ex:2017 + verificação de erro + '\0'
	if (!checkYear(year)){
		cleanScreen();
		fprintf(stdout, "Erro! Ano não válido.\n\n\n");
		return;
	}


	struct fileStructure file_salesdb = openFile("salesdb", "READ");

	if (!file_salesdb.filePointer)
		return;


	char fileMonth[MONTH_MAX_SIZE+1], fileYear[YEAR_MAX_SIZE+1];
	int fileQuantity;
	long int nr_of_unit_sold = 0;
	while (fscanf(file_salesdb.filePointer, "%[^;];%d;%*[^;];%*[^/]/%[^/]/%s\n", fileCode, &fileQuantity, 
		fileMonth, fileYear)!=EOF){

		if (!strcmp(code, fileCode) && !strcmp(month, fileMonth) && 
			!strcmp(year, fileYear))

			nr_of_unit_sold += fileQuantity;
	}

	closeFile(file_salesdb);

	if ( !nr_of_unit_sold){
		cleanScreen();
		fprintf(stdout, "Erro! O produto com o código \"%s\" não foi encontrado no registo de vendas na data referida.\n\n\n", code);
		return;
	}


	cleanScreen();      //limpar o ecra para ficar bonito
	fprintf(stdout, "Data de estatísticas: %s/%s \n\n", month, year);
	fprintf(stdout, "Número de unidades vendidas do produto com o código \"%s\" na data referida: %ld\n", code, nr_of_unit_sold);
	fprintf(stdout, "Lucro: %.2lf €\n\n\n", filePrice*(float)nr_of_unit_sold);
}


void topMonthlyStats(){

	char month[MONTH_MAX_SIZE+2], year[YEAR_MAX_SIZE+2];

	fputs("-----Estatísticas Mensais (TOP)-----\n\n\n", stdout);
	fputs("Data para processamento de estatística\n\n", stdout);

	fputs("Mes: ", stdout);
	getString(month, MONTH_MAX_SIZE+2);
	if (!checkMonth(month)){
		cleanScreen();
		fprintf(stdout, "Erro! Mes não válido.\n\n\n");
		return;
	}

	fputs("Ano: ", stdout);
	getString(year, YEAR_MAX_SIZE+2);      //6 para ex:2017 + verificação de erro + '\0'
	if (!checkYear(year)){
		cleanScreen();
		fprintf(stdout, "Erro! Ano não válido.\n\n\n");
		return;
	}


	struct fileStructure file_salesdb = openFile("salesdb", "READ");

	if (!file_salesdb.filePointer)
		return;


	char fileMonth[MONTH_MAX_SIZE+1], fileYear[YEAR_MAX_SIZE+1];
	int linhas=0;      //número máximo de posições ocupadas (número máximo de produtos diferentes vendidos na data referida)
	while (fscanf(file_salesdb.filePointer, "%*[^;];%*d;%*[^;];%*[^/]/%[^/]/%s\n", fileMonth, fileYear)!=EOF){

		if (!strcmp(month, fileMonth) && !strcmp(year, fileYear))

			linhas++;
	}

	if ( !linhas){
		closeFile(file_salesdb);
		cleanScreen();
		fputs("Erro! Não foram encontrados registos de vendas para esse dia.\n\n\n", stdout);
		return;
	}


	struct salesData tempData[linhas];

	//voltar com o pointer para o início do ficheiro para usar no fscanf
	fseek(file_salesdb.filePointer, 0, SEEK_SET);

	int linhas2=0, i;      //linhas2 = número de produtos diferentes vendidos naquele dia
	bool found=false;
	char fileCode[CODE_SIZE+1];
	int fileQuantity;
	//meter as informações do ficheiro dentro do array de estruturas
	while (fscanf(file_salesdb.filePointer, "%[^;];%d;%*[^;];%*[^/]/%[^/]/%s\n", fileCode, &fileQuantity, 
		fileMonth, fileYear)!=EOF){

		if (!strcmp(fileMonth, month) && !strcmp(fileYear, year)) {

			found=false;
			for (i=0; i<linhas2; i++){
				if (!strcmp(fileCode, tempData[i].code)){
					found=true;
					break;
				}
			}
			if (found)
				tempData[i].quantity+=fileQuantity;
			else {
				strcpy(tempData[linhas2].code, fileCode);
				tempData[linhas2].quantity = fileQuantity;
				linhas2++;
			}
		}
	}

	//se metermos closeFile(file_salesdb); dá segfault
	file_salesdb.lock->l_type = F_UNLCK;

	fcntl(file_salesdb.fd, F_SETLKW, file_salesdb.lock);

	//se o ponteiro for nulo, não fazer isto ou dá falha de segmentação
	if (file_salesdb.filePointer)
		fclose(file_salesdb.filePointer);

	close(file_salesdb.fd);

	//ordenar por quantidades vendidas
	struct salesData tempData_2;
	for (i=0; i<linhas2; i++){
		for (int j=i; j<linhas2; j++){
			if (tempData[j].quantity > tempData[i].quantity){
				tempData_2=tempData[i];
				tempData[i]=tempData[j];
				tempData[j]=tempData_2;
			}
		}
	}


	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;
	
	char fileName[NAME_MAX_SIZE+1];
	float filePrice;
	found=false;

	cleanScreen();
	fprintf(stdout, "Data de estatísticas: %s/%s \n\n", month, year);

	for (i=0; i<linhas2; i++){

		fprintf(stdout, "#%d\n", i+1);
		fprintf(stdout, "Código: %s\n", tempData[i].code);      //imprimir código
		fseek(file_productdb.filePointer, 0, SEEK_SET);      //andar com o ponteiro do ficheiro para o inicio do ficheiro
		found=false;

		while(fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", fileCode, fileName, 
			&fileQuantity, &filePrice)!=EOF){      //ir buscar informações do produto à base de dados de produtos

			if (!strcmp(tempData[i].code, fileCode)){
				fprintf(stdout, "Nome: %s\n", fileName);
				fprintf(stdout, "Quantidade disponível: %d\n", fileQuantity);
				fprintf(stdout, "Preço por unidade: %.2f €\n", filePrice);
				found=true;
				break;
			}
		}
		if (!found)
			fputs("Produto não existente na base de dados.\n", stdout);

		fprintf(stdout, "Unidades vendidas na data referida: %d\n", tempData[i].quantity);

		if (found)
			fprintf(stdout, "Lucro: %.2lf €\n\n\n", filePrice*(float)tempData[i].quantity);
	}


	closeFile(file_productdb);
}

bool checkNewOption(char option[]){
	if (strlen(option)>1)
		return false;

	return true;
}

bool checkNewName(char name[]){
	//Nome completo (não pode conter ';' e tem que ter entre 6 a 50 letras)
	if (strlen(name)<NAME_MIN_SIZE || strlen(name)>NAME_MAX_SIZE)
		return false;

	for (int i=0; name[i]!='\0'; i++)
		if (name[i]==';')
			return false;

	return true;
}

bool checkNewUsername(char username[]){
	//Username (6 a 20 caracteres e não são permitidos ';' nem espaços)
	if (strlen(username)<USERNAME_MIN_SIZE || strlen(username)>USERNAME_MAX_SIZE)
		return false;

	for (int i=0; username[i]!='\0'; i++)
		if (username[i]==';' || username[i]==' ')
			return false;

	return true;
}

bool checkNewPass(char password[]){
	//Password (6 a 20 caracteres e não são permitidos ';' nem espaços)
	if (strlen(password)<PASSWORD_MIN_SIZE || strlen(password)>PASSWORD_MAX_SIZE)
		return false;

	for (int i=0; password[i]!='\0'; i++)
		if (password[i]==';' || password[i]==' ')
			return false;

	return true;
}

bool checkNewEmail(char email[]){
	//Endereço de e-mail: (não pode conter ';' e tem que ter entre 6 (a@a.pt) a 50 letras)
	if (strlen(email)<6 || strlen(email)>EMAIL_MAX_SIZE)
		return false;

	int i=0;
	int domain_cnt=0;
	int domain_cnt2=0;
	int domain_start;

	for (; email[i]!='@' && email[i]!='\0'; i++){      //ir até ao @ ou ao fim se não existir @
		if (email[i]==';')
			return false;
	}

	if (i==0)      //falso se não existir nada antes do @ ou não existir @
		return false;

	for (; email[i]!='.'; i++){      //ir até ao 1º ponto
		if (email[i]==';')
			return false;
		domain_cnt++;
	}

	if (domain_cnt==0)      //falso se o nome de domínio antes do .com.pt, .com ou .pt for vazio
		return false;

	for (; email[i]!='\0'; i++){      //ir até ao fim
		if (email[i]==';')
			return false;
		domain_cnt2++;
	}

	domain_start=i-domain_cnt2+1;      //i - domain_cnt2 = posição do 1º ponto   +1 = posição a seguir ao 1º ponto
	if (domain_cnt2==3){
		if (email[domain_start]=='p' && email[domain_start+1]=='t' && email[domain_start+2]=='\0')      //se for do tipo .pt (3 caracteres)
			return true;
	}

	else if (domain_cnt2==4){
		if (email[domain_start]=='c' && email[domain_start+1]=='o' && email[domain_start+2]=='m' && email[domain_start+3]=='\0')      //se for do tipo .com (4 caracteres)
			return true;
	}

	else if (domain_cnt2==7){
		if (email[domain_start]=='c' && email[domain_start+1]=='o' && email[domain_start+2]=='m' && email[domain_start+3]=='.'
			&& email[domain_start+4]=='p' && email[domain_start+5]=='t' && email[domain_start+6]=='\0')      //se for do tipo .com.pt (7 caracteres)
			return true;
	}


	return false;
}

bool checkNewContact(char contact[]){
	//Contacto telefónico (conter 9 (número local) ou 12 números (número local + código de pais)):
	if (strlen(contact)!=CONTACT_SIZE_1 && strlen(contact)!=CONTACT_SIZE_2)
		return false;

	for (int i=0; contact[i]!='\0'; i++)
		if (contact[i]<'0' && contact[i]>'9')
			return false;

	return true;
}

bool checkNewBalance(char stringBalance[]){
	//Montande inicial da conta bancária (entre 0.0 e 340282346638528859811704183484516925440.0, até 2 casas decimais e usar '.' em vez de ',')

	//se o dinheiro tiver mais do que 42 caracteres (max int float + . + 2 casas decimais)
	if (strlen(stringBalance)>MONEY_MAX_SIZE_STRING)
		return false;

	int dot_cnt=0, integer=0, decimal=0;
	for (int i=0; stringBalance[i]!='\0'; i++){
		if (stringBalance[i]<'0' || stringBalance[i]>'9'){
			if (stringBalance[i]=='.')
				dot_cnt++;      //se apanhar um ponto ou vírgula, aumentar contador de pontos
			else
				return false;
		}
		
		if (dot_cnt>1)      //se tiver mais do que um ponto ou vírgula
			return false;
		else if (dot_cnt==0)
			integer++;
		else if (dot_cnt==1)
			decimal++;
		else if (integer>39)      //se tiver mais do que 39 caracteres antes do ponto ou vírgula
			return false;	
		else if (decimal>2){      //se tiver mais do que duas casas decimais
			return false;
		}
	}

	float balance = stringToFloat(stringBalance);

	//340282346638528859811704183484516925440.0 = max float
	//balance<0 para ver se excedeu o limite de float
	if (balance>MONEY_MAX_SIZE_FLOAT || balance<0)
		return false;

	//se chegou a este ponto é porque o saldo está certo
	//se não existirem duas casas decimais, meter-las
	if (dot_cnt==0){
		int len = (int)strlen(stringBalance);
		stringBalance[len]='.';
		stringBalance[len+1]='0';
		stringBalance[len+2]='0';
		stringBalance[len+3]='\0';
	}
	else if (dot_cnt==1){

		if (decimal==0){
			int len = (int)strlen(stringBalance);
			stringBalance[len]='0';
			stringBalance[len+1]='0';
			stringBalance[len+2]='\0';
		}
		else if (decimal==1){
			int len = (int)strlen(stringBalance);
			stringBalance[len]='0';
			stringBalance[len+1]='\0';
		}
	}

	return true;
}

bool checkNewPerms(char perms[]){
	//Permissões -> 1 = Administrador, 2 = Utilizador
	if (strlen(perms)!=PERMS_MAX_SIZE)
		return false;

	if (perms[0]!='1' && perms[0]!='2')
		return false;

	return true;
}

bool checkNewCode(char code[]){
	if (strlen(code)==CODE_SIZE){      //se o código tiver 9 caracteres, verificar se são todos algarismos
		for (int i=0; i<9; i++){
			if (code[i]<'0' || code[i]>'9')
				return false;
		}
		return true;      //caso o código tenha 9 algarismos e não exista nenhum caracter sem ser algarismo
	}
	else if (strlen(code)==0)      //se o código for vazio (usar código aleatório)
		return true;

	return false;
}

bool checkNewQuantity(char stringQuantity[]){
	if (strlen(stringQuantity)>QUANTITY_MAX_SIZE_STRING)
		return false;

	for (int i=0; stringQuantity[i]!='\0'; i++)
		if (stringQuantity[i]<'0' || stringQuantity[i]>'9')
			return false;

	if (stringToInt(stringQuantity)<0 || stringToInt(stringQuantity)>QUANTITY_MAX_SIZE_INT)
		return false;

	return true;
}

bool checkConfirmation(char confirmation[]){
	if ( !strcmp(confirmation, "SIM"))
		return true;
	else if ( !strcmp(confirmation, "S"))
		return true;
	else if ( !strcmp(confirmation, "sim"))
		return true;
	else if ( !strcmp(confirmation, "s"))
		return true;
	else
		return false;

	return false;
}

bool checkDay(char day[]){
	if (strlen(day)>2 || strlen(day)==0){
		return false;
	}
	else if (strlen(day)==1){
		if (day[0]=='0')
			return false;
	}
	else if (strlen(day)==2){
		if (day[0]=='0' && day[1]=='0')
			return false;
		else if (day[0]=='3' && day[1]>'1')
			return false;
		else if (day[0]>'3')
			return false;
	}

	if (strlen(day)==1){      //meter 0 atras para comparar mais facilmente
		day[2]='\0';
		day[1]=day[0];
		day[0]='0';
	}

	return true;
}

bool checkMonth(char month[]){
	if (strlen(month)>2 || strlen(month)==0){
		return false;
	}
	else if (strlen(month)==1){
		if (month[0]=='0')
			return false;
	}
	else if (strlen(month)==2){
		if (month[0]=='0' && month[1]=='0')
			return false;
		else if (month[0]=='1' && month[1]>'2')
			return false;
		else if (month[0]>'1')
			return false;
	}

	if (strlen(month)==1){
		month[2]='\0';
		month[1]=month[0];
		month[0]='0';
	}

	return true;
}

bool checkYear(char year[]){
	if (strlen(year)!=4)
		return false;

	for (int i=0; year[i]!='\0'; i++)
		if (year[i]<'0' || year[i]>'9')
			return false;

	return true;
}

//original = advancedlinuxprogramming.com/listings/chapter-8/lock-file.c
/*Avé Google, StackOverflow, AdvancedLinuxProgramming e 1 dia inteiro por estas duas maravilhas!!!
Metade nosso (desenvolvedores), metade internet <3 */
struct fileStructure openFile(char fileToEdit[], char mode[]){

	struct fileStructure fileStruct = {0, (struct flock *)NULL, (FILE *)NULL};

	int fd;
	struct flock lock;


	//modo de leitura
	if ( !strcmp(mode, "READ")){

		/*abre um file descriptor para o ficheiro que queremos e mete um pointer para o ficheiro na estrutura de ficheiros
		O_RDONLY = read-only
		O_WRONLY = write-only
		O_RDWR = read/write */
		//http://codewiki.wikidot.com/c:system-calls:open
		fd = open(fileToEdit, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		
		/*passar um file descriptor para um file stream
		r 	Opens an existing text file for reading purpose.
		
		w 	Opens a text file for writing. If it does not exist, 
		then a new file is created. Here your program will start writing content from the beginning of the file.
		
		a 	Opens a text file for writing in appending mode. 
		If it does not exist, then a new file is created. 
		Here your program will start appending content in the existing file content.
		
		r+ 	Opens a text file for both reading and writing.
		
		w+ 	Opens a text file for both reading and writing. 
		It first truncates the file to zero length if it exists, otherwise creates a file if it does not exist.
		
		a+ 	Opens a text file for both reading and writing. I
		t creates the file if it does not exist. 
		The reading will start from the beginning but writing can only be appended.*/
		fileStruct.filePointer = fdopen(fd, "r");

		//iniciar a estrutura do flock
		memset(&lock, 0, sizeof(lock));

		/*F_RDLCK = lock de leitura
		F_WRLCK = lock de escrita
		F_UNLCK = tirar o lock (unlock)*/
		lock.l_type = F_RDLCK;
	}
	//modo de escrita
	else if ( !strcmp(mode, "WRITE")){
		//O_TRUNC = eliminar tudo o que está dentro do ficheiro
		fd = open(fileToEdit, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		fileStruct.filePointer = fdopen(fd, "r+");
		memset(&lock, 0, sizeof(lock));
		lock.l_type = F_WRLCK;
	}
	//modo de acrescentar ao final
	else if ( !strcmp(mode, "APPEND")){
		fd = open(fileToEdit, O_CREAT | O_RDWR | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		fileStruct.filePointer = fdopen(fd, "a");
		memset(&lock, 0, sizeof(lock));
		lock.l_type = F_WRLCK;
	}
	else {
		cleanScreen();
		fputs("Erro! Modo de abrir ficheiro não reconhecido.\n\n\n", stderr);
		fileStruct.filePointer=NULL;
		return fileStruct;
	}
	
	//meter um lock no ficheiro e ao mesmo tempo verificar se dá para meter e caso passe 3 segundos sem poder meter, então dar erro
	int cnt=0;
	while (fcntl(fd, F_SETLK, &lock)==-1){
		sleep(1);
		cnt++;
		if (cnt>=3){
			cleanScreen();
			fprintf(stderr, "Não foi possível meter um lock no ficheiro \"%s\"\n\n\n", fileToEdit);
			fileStruct.filePointer=NULL;
			return fileStruct;
		}
	}

	//meter as informações nesta estrutura para usar noutras funções
	fileStruct.fd = fd;
	fileStruct.lock = &lock;

	if (!fileStruct.filePointer) {
		closeFile(fileStruct);
		cleanScreen();
		fprintf(stderr, "Erro! Não foi possível abrir o ficheiro \"%s\".\n\n\n", fileToEdit);
		return fileStruct;      //retornar na mesma a estrutura para fazer return também na função de chamada
	}

	return fileStruct;
}

void closeFile(struct fileStructure fileStruct){

	//retirar o lock

	/*F_RDLCK = lock de leitura
	F_WRLCK = lock de escrita
	F_UNLCK = tirar o lock (unlock)*/
	fileStruct.lock->l_type = F_UNLCK;

	fcntl(fileStruct.fd, F_SETLKW, fileStruct.lock);

	//se o ponteiro for nulo, não fazer isto ou dá falha de segmentação
	if (fileStruct.filePointer)
		fclose(fileStruct.filePointer);

	close(fileStruct.fd);
}

void logUser(char username[]){
	struct fileStructure file_userdb = openFile("userdb", "WRITE");
	struct fileStructure file_temp = openFile("temp", "APPEND");

	struct userData tempUserData;

	while (fscanf(file_userdb.filePointer, "%[^;];%[^;];%[^;];%[^;];%[^;];%f;%[^;];%s\n", tempUserData.name, tempUserData.username, 
		tempUserData.password, tempUserData.email, tempUserData.contact, &tempUserData.balance, tempUserData.perms, tempUserData.logged)!=EOF) {

		if ( !strcmp(tempUserData.username, username))
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;TRUE\n", tempUserData.name, tempUserData.username, tempUserData.password, 
				tempUserData.email, tempUserData.contact, tempUserData.balance, tempUserData.perms);

		else
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempUserData.name, tempUserData.username, tempUserData.password, 
				tempUserData.email, tempUserData.contact, tempUserData.balance, tempUserData.perms, tempUserData.logged);
	}

	rename("temp", "userdb");
	closeFile(file_userdb);
	closeFile(file_temp);
}

void logoutUser(char username[]){
	struct fileStructure file_userdb = openFile("userdb", "WRITE");
	struct fileStructure file_temp = openFile("temp", "APPEND");

	struct userData tempUserData;

	while (fscanf(file_userdb.filePointer, "%[^;];%[^;];%[^;];%[^;];%[^;];%f;%[^;];%s\n", tempUserData.name, tempUserData.username, 
		tempUserData.password, tempUserData.email, tempUserData.contact, &tempUserData.balance, tempUserData.perms, tempUserData.logged)!=EOF) {

		if ( !strcmp(tempUserData.username, username))
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;FALSE\n", tempUserData.name, tempUserData.username, tempUserData.password, 
				tempUserData.email, tempUserData.contact, tempUserData.balance, tempUserData.perms);

		else
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempUserData.name, tempUserData.username, tempUserData.password, 
				tempUserData.email, tempUserData.contact, tempUserData.balance, tempUserData.perms, tempUserData.logged);
	}

	rename("temp", "userdb");
	closeFile(file_userdb);
	closeFile(file_temp);
}

void generateCode(char code[]){
	srand( (unsigned int)time(NULL));      //usar o srand com uma seed para gerar números aleatórios

	for (int i=0; i<9; i++)      //gerar números de 0 a 9 para cada posição do código
		code[i]= (char)(rand()%10 + '0');
			
	code[9]='\0';
}

bool getString(char string[], int size){
	char *fgr;
	bool result = true;      //Assumir como tudo correto

	fgr = fgets(string, size, stdin);

	if (fgr != NULL){
		if (string[strlen(string)-1] != '\n')
			flushInput();
	}
	else
		result = false;      //fgets retornou um erro ou leu EOF sem ler mais nada

	removeNewLine(string);      //transformar '\n' para '\0'

	return result;      //valor de retorno caso queiramos fazer debugging
}

void flushInput(){
	int ch;      //a função getchar retorna um int
	ch = getchar();

	while ( ch!=EOF && ch!='\n' )
		ch = getchar();      //ler caracteres até ao fim do buffer

	clearerr(stdin);      //limpar o EOF e indicador de erros que está no final do buffer stdin
}

void removeNewLine(char string[]){
	for (int i=0; string[i]!='\0'; i++)
		if (string[i]=='\n')
			string[i]='\0';
}

void cleanScreen(){
	for (int i=0; i<100; i++)
		fputs("\n", stdout);
}

float stringToFloat(char string[]){
	int dotpos=0;
	int len = (int)strlen(string);
	float number=0.0;
	
	for (int i = 0; i < len; i++) {
		if (string[i] == '.') {
			dotpos = len - i - 1;
		}
		else {
			number = number * 10.0f + (float)(string[i]-'0');
		}
	}

	while ( dotpos--) {
		number /= 10.0f;
	}

	return number;
}

int stringToInt(char string[]){
	int quantity = atoi(string);

	return quantity;
}

void about(){
	puts("Servidor de Gestão de SuperMercado.\n");
	puts("Feito por Frederico Emanuel e Fernando Silva no âmbito do projeto da cadeira");
	puts("de 1º ano, Laboratório de Computadores, da Licenciatura em Ciência de");
	puts("Computadores e Mestrado Integrado em Engenharia de Redes e Sistemas Informáticos");
	puts("na mui nobre Faculdade de Ciências da Universidade do Porto.\n\n");
	puts("Frederico Emanuel Almeida Lopes, up201604674");
	puts("Fernando Daniel Sousa Ventura da Silva, up201604670\n\n\n");
}
