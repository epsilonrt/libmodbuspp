# Format des fichiers JSON pour modbuspp

modbusspp utilise le format JSON (JavaScript Object Notation) pour décrire les maîtres, les serveurs et les routeurs Modbus. Ce format est léger, facile à lire et à écrire, et largement utilisé pour l'échange de données. Il est basé sur une syntaxe simple qui permet de représenter des objets et des tableaux.

Ces fichiers permettent la configuration d'un programme modbuspp, en l'adaptant à l'environnement matériel et aux besoins de l'utilisateur sans avoir à modifier le code source (et donc à recomplier), ou à passer des paramètres en ligne de commande.

Voici les principaux éléments de la syntaxe JSON :

- Les données sont présentées sous forme de paires clé/valeur séparées `:`   
- Les éléments sont séparés par des virgules   
- Les accolades {} désignent les objets  
- Les crochets [] désignent des tableaux  

L'ensemble d'un fichier JSON est un objet, anonyme, donc encadré par des accolades et il contient les objets. Un objet est précédé par une clé, qui est une chaîne de caractères, suivie de deux points `:` et de la valeur associée. Dans modbuspp, les objets sont utilisés pour décrire les :

- maîtres qui sont des objets gérés par la classe `Master` et ses esclaves `Slave`, 
- serveurs qui sont des objets gérés par la classe `Server` et ses esclaves `BufferedSlave`,
- routeurs qui sont des objets gérés par la classe `Router` qui est une extension de la classe `Server`.

Ces trois types d'objets sont des `Device`  qui partagent des propriétés communes.

Chaque objet peut contenir des propriétés qui seront ignorées par modbuspp, mais qui peuvent être utiles pour l'utilisateur. Par exemple, on peut ajouter une propriété `name` pour identifier un maître, un serveur ou un routeur. JSON ne prennant pas en charge les commentaires, on peut utiliser une propriété pour ajouter des informations supplémentaires. Il est d'usage de commencer ces propriétés par un underscore `_` pour indiquer qu'elles sont spécifiques à l'utilisateur et non à modbuspp. Par exemple, on peut ajouter une propriété `_comment` pour ajouter un commentaire explicatif.

A noter que pour comprendre la structure des objets JSON, il est utile de se référer à la documentation de modbuspp, qui décrit les classes et les fonctions associées. Les objets JSON sont utilisés pour configurer ces classes et leurs instances.


Vous pouvez lire ce document dans sa version [en français](https://github.com/epsilonrt/libmodbuspp/blob/master/doc/modbuspp_json_fr.md)

## Device

Un Device est un objet JSON qui contient les informations sur un appareil Modbus, il décrit une liaison Modbus, en voilà un exemple:  

```json
{
  "example-device": {
    "mode": "rtu",
    "connection": "/dev/tnt0",
    "settings": "38400E1",
    "debug": true,
    "response-timeout": 500,
    "byte-timeout": 500,
    "rtu": {
      "mode": "rs232"
    },
    "_comment": "Ceci est un exemple simple, mais incomplet, d'un maître Modbus RTU."
  }
}
```
Dans cet exemple, on a un objet JSON qui décrit un device Modbus RTU identifié par la clé `example-device` contenu dans l'objet racine. Il contient plusieurs champs qui décrivent la liaison Modbus, ainsi qu'un objet `rtu` pour les paramètres spécifiques au mode RTU.

Ces champs sont utilisés pour décrire la liaison Modbus d'un maître (Classe `Master`), d'un serveur (Classe `Server`) ou d'un routeur (Classe `Router`). 

Les champs `mode`, `connection`, `settings` sont obligatoires:  

- `mode`: Mode de communication, peut être `rtu` ou `tcp`. Lié à l'énumération `Net`.  
  - `rtu`: pour une liaison série Modbus RTU.  
  - `tcp`: pour une liaison TCP/IP Modbus TCP.
- `connection`: Chemin de la connexion série, adresse IP (v4 ou v6) ou nom d'hôte pour TCP.  Pour un serveur TCP, on peut utiliser `*` pour écouter sur toutes les interfaces. Pour une connexion série, c'est généralement un chemin comme `/dev/ttyS1` ou `/dev/ttyUSB0`, `COM1`, etc.  
- `settings`: Paramètres de la connexion série, par exemple `38400E1` pour 38400 bauds, 8 bits de données, pas de parité, 1 bit d'arrêt. Numéro de port pour TCP.  

La fonction liée à ces 3 champs est `Device::setBackend()`

Les autres champs sont optionnels, voici leur description :  

- `debug`: Si `true`, active le mode débogage pour afficher les requêtes et réponses Modbus. La fonction liée est `Device::setDebug()`.  
- `response-timeout`: Délai d'attente pour la réponse en millisecondes. La fonction liée est `Device::setResponseTimeout()`.  
- `byte-timeout`: Délai d'attente pour chaque octet reçu en millisecondes. La fonction liée est `Device::setByteTimeout()`.
- `rtu`: Objet pour les paramètres spécifiques au mode RTU.
  - `mode`: Mode de la ligne RS485, peut être `rs485` ou `rs232`. La fonction liée est `Device::setSerialMode()`.
  - `rts`: État de la ligne RTS, peut être `up` ou `down` ou `none`, par défaut `none`. La fonction liée est `Device::setRts()`.  
  - `rts-delay`: Délai en millisecondes pour la ligne RTS. La fonction liée est `Device::setRtsDelay()`.  
- `recovery-link`:  active la reconnection automatique en cas de perte de liaison. La fonction liée est `Device::setRecoveryLink()`.  

## Master

Un maître est un Device qui envoie des requêtes Modbus à un ou plusieurs esclaves de la classe `Slave`, le programme pourra donc effectuer toutes les opérations inhérantes à cette classe : lire et écrire des registres, des entrées, des bobines, etc. Voici un exemple de maître Modbus RTU avec plusieurs esclaves :  

```json
{
  "modbuspp-master": {
    "name": "rs485",
    "mode": "rtu",
    "connection": "/dev/ttyS1",
    "settings": "38400E1",
    "debug": true,
    "response-timeout": 500,
    "byte-timeout": 500,
    "rtu": {
      "mode": "rs485",
      "rts": "down"
    },
    "slaves": [
      {
        "id": 32
      },
      {
        "id": 33
      },
      {
        "id": 34,
        "pdu-adressing": true
      },
      {
        "id": 35
      }
    ]
  }
}
```

Dans cet exemple, le maître est configuré pour communiquer avec quatre esclaves ayant les identifiants 32, 33, 34 et 35. La fonction liée à ces champs est `Master::addSlave()`.

Chaque objet dans le tableau `slaves` représente un esclave Modbus qui est identifié par son `id` (entre 1 et 247). Il est possible d'ajouter une propriété `pdu-adressing` pour spécifier le mode d'adressage PDU (adressage données commençant à 0) . La fonction liée est `Master::setPduAddressing()`.

Un maître n'a rien d'autre à configurer que la liaison Modbus, et la liste des esclaves avec lesquels il communique. Il n'a rien d'autres à connaitre que l'identifiant de chaque esclave, et éventuellement le mode d'adressage PDU. Il n'y a pas de configuration pour les tables de données, car un maître ne gère pas les données, il se contente de les lire ou de les écrire dans les esclaves.

## Server

Un serveur est un Device qui reçoit des requêtes Modbus d'un maître auquel il est connecté. Un serveur implémente un ou plusieurs esclaves de la classe `BufferedSlave`, qui eux-même implémentent les tables de données Modbus: registres d'entrée (`input-register`), registres de maintien (`holding-register`), bobines (`coil`) et entrées discrètes (`discrete-input`). 

La classe `Server` associée à la classe `BufferedSlave` permet donc de réaliser des esclaves Modbus implémentés sous forme de logiciels, qui peuvent être configurés par un fichier JSON.

Voici un exemple de serveur Modbus TCP avec un esclave :  

```json
{
  "modbuspp-server": {
    "mode": "tcp",
    "connection": "localhost",
    "settings": "1502",
    "debug": true,
    "recovery-link": true,
    "response-timeout": 500,
    "byte-timeout": 500,
    "slaves": [
      {
        "id": 10,
        "blocks": [
          {
            "table": "holding-register",
            "quantity": 4,
            "data-type": "float",
            "starting-address": 1,
            "endian" : "cdab",
            "values" : [1.5,-3.14,5.23e12,1.63e-6]
          },
          {
            "table": "input-register",
            "quantity": 2,
            "values" : [101,"0x100"]
          },
          {
            "table": "coil",
            "quantity": 12,
            "values" : ["0x5A",true,1,false,0]
          },          
          {
            "table": "discrete-input",
            "quantity": 4,
            "values" : [0,0,1,1]
          }
        ]
      }
    ]  
  }
}
```

La première partie du fichier reprend les champs décrits dans Device. Ces paramètres sont suivis par un tableau `slaves` qui contient les esclaves implémentés par le serveur. Ici, nous en voyons un seul avec l'identifiant `10`. Celui-ci est configuré pour implémenter quatre tables de données : `holding-register`, `input-register`, `coil` et `discrete-input`. Ces tables sont décrites par des objets dans le tableau `blocks`, chacun ayant les champs suivants :  

- `table`: Le type de données de la table, qui peut être `holding-register`, `input-register`, `coil` ou `discrete-input`. **Ce champ est obligatoire pour toutes les tables**. 
- `quantity`: Le nombre d'éléments dans la table, c'est-à-dire le nombre de registres, d'entrées, de bobines ou d'entrées discrètes. **Ce champ est obligatoire pour toutes les tables**.  
- `starting-address`: L'adresse de départ pour les registres, uniquement pour les tables `holding-register` et `input-register`. Si non spécifié, l'adresse de départ est 1 (0 si l'esclave est en mode PDU). 
- `data-type`: Le type de données pour les registres, peut être `uint16`, `uint32`, `uint64`, `int16`, `int32`, `int64`, `float`, `double` et `longdouble`. C'est la classe modèle `Data` qui gère ces types de données. A noter que ces types stockent uniquement des valeurs numériques dont la taille minimale est de 2 octets. Par défaut, le type de données est `uint16`.  
- `endian`: L'endianness des données pour les registres, peut être `abcd`, `cdab`, `badc` et `dcba`. La valeur par défaut est `abcd`.  La fonction liée est `Data::setEndianness()`.  
- `values`: Un tableau de valeurs initiales pour la table. Les valeurs peuvent être des entiers, des flottants pour les registres `holding-register` et `input-register`. Pour les tables `coil` et `discrete-input`, les valeurs peuvent être des booléens (`true` ou `false`) ou des entiers (`0` ou `1`) ou des valeurs hexadécimales (ex: `0x5A`). 

Il doit y avoir **au moins un élément dans le tableau `blocks`**, et chaque bloc doit avoir au moins les champs `table` et `quantity`. Les autres champs sont optionnels.

La description des esclaves implémentés est bien plus complète que pour un maître, car un serveur peut implémenter plusieurs tables de données. La fonction liée à ces champs est `Server::addSlave()` pour ajouter un esclave, et `BufferedSlave::addBlock()` pour ajouter une table de données à un esclave.

## Router

Un routeur est un Device qui permet d'implémenter plusieurs maîtres reliés à un serveur qui attend les requêtes des maîtres "extérieurs" et aiguille les requêtes vers le bons maître en fonction de l'identifiant de l'esclave demandé. Il est donc possible d'avoir un routeur Modbus TCP ou RTU qui communique avec plusieurs maîtres qui peuvent être en mode RTU ou TCP.

Un routeur dispose au moins de 2 connexions : une connexion vers l'extérieur sur laquelle le routeur écoute les requêtes des maîtres (équivalent au port WAN des routeurs TCP), et une connexion vers l'intérieur sur laquelle il communique comme un maître avec un ou plusieurs esclaves (équivalent au port LAN des routeurs TCP).  On peut y ajouter d'autres connexions intérieures.

Afin de pouvoir gérer les requêtes effectuées depuis l'extérieur, et évenuellement indiquer au maître distant que tel registre n'est pas accessible, le routeur doit connaître la carte mémoire de chaque esclaves qu'il gère. Il doit donc savoir quels sont les esclaves qui lui sont connectés, et quels sont les registres, entrées, bobines et entrées discrètes que ses esclaves gèrent.

Voici un exemple de routeur Modbus qui dispose de trois connexions : une connexion TCP vers l'extérieur, et deux connexions série vers l'intérieur:  

```json
{
  "modbuspp-router": {
    "mode": "tcp",
    "connection": "localhost",
    "settings": "1502",
    "recovery-link": true,
    "debug": true,
    "response-timeout": 500,
    "byte-timeout": 500,
    "masters": [
      {
        "name": "rs485",
        "mode": "rtu",
        "connection": "/dev/ttyS1",
        "settings": "38400E1",
        "debug": true,
        "response-timeout": 500,
        "byte-timeout": 500,
        "rtu": {
          "mode": "rs485",
          "rts": "down"
        },
        "slaves": [
          {
            "id": 33,
            "blocks": [
              {
                "table": "input-register",
                "quantity": 6
              },
              {
                "table": "holding-register",
                "quantity": 8
              }
            ]
          }
        ]
      },
      {
        "name": "virtual-clock",
        "mode": "rtu",
        "connection": "/dev/tnt0",
        "settings": "38400E1",
        "debug": true,
        "response-timeout": 3000,
        "byte-timeout": 500,
        "slaves": [
          {
            "id": 10,
            "blocks": [
              {
                "table": "input-register",
                "quantity": 8
              },
              {
                "table": "holding-register",
                "quantity": 2
              },
              {
                "table": "coil",
                "quantity": 1
              }
            ]
          }
        ]
      }
    ]
  }
}
```

La première partie du fichier reprend les champs décrits dans Device et correspond à la configuration de la connexion vers l'extérieur.  
Viens ensuite un tableau `masters` qui contient connexions vers l'intérieur, chaque objet dans le tableau représente un maître Modbus qui est identifié par son `name` (chaîne de caractères). **Le champs `name` est obligatoire pour identifier chaque maître***.

Chaque objet dans le tableau `masters` contient une description de la connexion vers l'intérieur (syntaxe identique à celle d'un Device), ainsi qu'un tableau `slaves` qui contient les esclaves connectés à ce maître. Chaque esclave est décrit par un objet dans le tableau `slaves` avec une syntaxe identique à celle d'un serveur (et non d'un maître, car comme indiqué précédement, le serveur connecté à l'extérieur a besoin de connaitre le mapping complet).

