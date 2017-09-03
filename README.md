# Supermarket
Project for the subject "Laboratório de Computadores" ("Computer Lab") of the degree of Computer Science and integrated master in Engineering in Networking and Information Systems in the Faculty of Sciences of the University of Porto that consists in making a server and a client for a supermarket controlled locally by files. The code and comments are in portuguese so the README is also in portuguese.

<br />
<br />

-----Manual de intruções dos programas de gestão do supermercado----- <br />

Conta padrão (automaticamente criada): <br />
Username: admin <br />
Password: admin <br />

<br />

#1 - Estes programas só funcionam em Linux e MacOS

#2 - Usar o programa é simples e direto bastando seguir as instruções que aparecem no ecra.
Dica: Usar em modo fullscreen

#3 - Ao executar os ficheiros cliente ou servidor, é automaticamente criado os ficheiros 
necessários à boa execução do programa

#4 - Ao executar o ficheiro servidor, se não existir nenhuma conta com permissões de administrador, 
é automaticamente criada a conta padrão (admin)

<br />

Ficheiros: <br />
userdb - Base de dados das informações dos utilizadores existentes

productdb - Base de dados dos produtos existentes

salesdb - Registo de vendas

usersdb/"USERNAME"/shoplist - Base de dados da lista de compras (carrinho) do utilizador 
com o username "USERNAME"

<br />

Comandos para compilar os ficheiros:

Linux:
server = gcc -Wall -Wextra -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all --param ssp-buffer-size=1 -D_FORTIFY_SOURCE=2 -O2 -Wl,-z,relro,-z,now server.c -o server_linux <br />
client = gcc -Wall -Wextra -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all --param ssp-buffer-size=1 -D_FORTIFY_SOURCE=2 -O2 -Wl,-z,relro,-z,now client.c -o client_linux <br />

Mac OS:
server = gcc -Wall -Wextra -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all --param ssp-buffer-size=1 -D_FORTIFY_SOURCE=2 -O2 server.c -o server_macos <br />
client = gcc -Wall -Wextra -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all --param ssp-buffer-size=1 -D_FORTIFY_SOURCE=2 -O2 client.c -o client_macos <br />

<br />

Explicação das opções de compilação: <br />
-Wall -Wextra <br />
Mostrar todos os avisos para garantir que o código está sem erros <br />

-Wconversion -Wsign-conversion <br />
Mostrar avisos de tipos de variáveis não compativeis <br />

-Wformat-security <br />
Mostrar avisos sobre o uso de funções que podem representar problemas de segurança <br />

-fstack-protector-all <br />
Ligar Canary (proteção de segurança) <br />

--param ssp-buffer-size=1 <br />
Tamanho mínimo necessário para um array ter a proteção de stack smashing quando a opção -fstack-protection é usada <br />

-D_FORTIFY_SOURCE=2 -O2 <br />
Ligar Fortify (o macro FORTIFY_SOURCE consegue detetar alguns buffers overflows em algumas funções que fazem operações com a memória e strings) <br />

-Wl,-z,relro,-z,now <br />
Ligar RELRO na proteção máxima (técnica de mitigação de corrupção de memória) <br />

 <br />

Comandos para executar os binários:

server = ./server <br />
client = ./client <br />

 <br />

Informações contidas nos ficheiros:

usesdb = NOME;USERNAME;PASSWORD;EMAIL;CONTACTO_TELEFÓNICO;SALDO;TIPO_DE_CONTA;AUTENTICADO

productdb - CÓDIGO;NOME;UNIDADES_DISPONÍVEIS;PREÇO

salesdb - CÓDIGO;QUANTIDADE;USERNAME_DE_QUEM_COMPROU;DIA/MES/ANO

usersdb/"USERNAME"/shoplist - CÓDIGO;QUANTIDADE

 <br />

Feito por Frederico Emanuel e Fernando Silva no âmbito do projeto da cadeira de 1º ano, Laboratório de Computadores, da Licenciatura em Ciência de Computadores e Mestrado Integrado em Engenharia de Redes e Sistemas Informáticos na mui nobre Faculdade de Ciências da Universidade do Porto.
Frederico Emanuel Almeida Lopes, up201604674
Fernando Daniel Sousa Ventura da Silva, up201604670
