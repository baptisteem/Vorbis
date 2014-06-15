##########################################################
#                                                        #
#               OGG / VORBIS - Projet C 2014             #
#                                                        #
##########################################################

Equipe :  QUESNOT Jean-Joseph
          LE ROUX Erwan
          EMMANUEL Baptiste

Contact : Si pendant l'execution du programme quelque chose devait
          très mal se passer, fichier manquant, pas de fichier du tout,
          mauvaise archivage, etc... il est possible de nous contacter
          à l'adresse baptiste.em@gmail.com 

--------------------------------------------------------
* Avancement du projet : 

  - Tous les modules ont été implémentés, ceux de bases et ceux optionnels.
  - Une implémentation de fast IMDCT a été faites. Celle ci n'est pas aussi 
    rapide que la version fourni. Cela viens, nous le pensons, de l'implémentation
    et non du modèle mathématique. Pour la tester il suffit de faire

    $ make fast 

---------------------------------------------------------
* Organistation du Makefile : 

Le Makefile a été en partie refait pour nous permettre de compiler
chaque module indépendement des autres. Ainsi un

    $ make residue

va nous créer un exécutable avec tout les objets de référence et remplacer
l'objet residue par notre module compilé. Cela est bien sur utilisable avec
tous les modules. Toujours pour residue, un programme vorbis_decoder_residue
sera crée.  
 
Pour utiliser tout les modules que nous avons implémentés il suffit de faire

    $ make
 
Un programme vorbis_decoder_all sera alors crée. Pour des soucis de vitesse, nous avons enlevé notre time_domain_transform dans la compilation totale. Il peut évident y être ajouté, et peut aussi être testé grâce au script de test.

Enfin si l'on veut utiliser seulement les modules de références, il suffit de faire

    $ make ref


----------------------------------------------------------
* Utilisation du script de test : 

Un script de test a été crée en python et permet de tester le RMS
de chaque module pour tous les exemples. Le script se trouve dans le dossier test/
et s'utilise ainsi

    $ python script_test.py

Il est ensuite demandé à l'utilisateur quel module il souhaite tester. Tous les sons
seront alors testés avec l'outil ../utils/rms 
Un rapport sera généré sous la forme rms_module_name.log, et contient le résultat du
test RMS.
Il est bien sur possible de tester l'ensemble du programme avec nos modules en
choisissant l'option 'Tout les modules'.

Organisation du dossier test/

  - generated_samples/  : dossier contenant les .ogg transformés en .wav par notre 
                          programme.
  - ogg_examples/       : dossier contenant les .ogg de bases fourni au début du 
                          projet.
  - wav_examples/       : dossier contenant les .wav crée à partir du programme de
                          référence. Utils pour comparer nos .wav à ceux là.

  /!\ Ne pas supprimer ces dossiers. Ils permettent au script de test de fonctionner 
      correctement. 

---------------------------------------------------------
