import subprocess
import os
import shutil

PROG_TEST = "../utils/rms"
PROG = "./vorbis_decoder"
MODULES = ["mode","dot_product","floor","time_transform","helpers"]
OGG_EXAMPLES = "ogg_examples/"
WAV_EXAMPLES = "wav_examples/"
DIR_GENERATED_SAMPLE = "generated_samples/"
LOG_FILE = "test.log"


def print_header():
  print("--------------------------------------------")
  print("|                                          |")
  print("|        Script de test - VORBIS           |")
  print("|                                          |")
  print("--------------------------------------------")
  print("")
  print("Auteurs : EMMANUEL Baptiste")
  print("          LE ROUX Erwan")
  print("          QUESNOT Jean-Joseph")
  print("")
  print("")
  print("Ce script permet de tester les modules un par un. Chaque module genere un executable avec seulement ce module compile en .o, le reste des librairies viennent de celles donnees au debut. Avec cet executable, des fichiers en .wav sont generes grace aux .ogg. Ils sont ensuite compares aux fichiers .wav de reference.")
  print("")

'''
  Affiche la liste des modules disponibles
'''
def display_menu():
  print("")
  print("Liste des modules disponibles : ")
  print("")
  i = 1
  print("0 - Tout les modules")
  for mod in MODULES:
    print("%d - %s" % (i,mod))
    i+=1
  print("")
  return raw_input("Quel module voulait vous tester ?  ")

'''
  Genere un programme vorbis_decode en fonction du module que l'on veut tester. Appelle adequat a make
'''
def generate_prog(mod):
  subprocess.call(["make " + mod + " -C ../"], shell=True)

'''
  Decompresse un son .ogg en .wav
'''
def generate_sample(sample, mod):
  gen_sample = DIR_GENERATED_SAMPLE + mod + "_" + sample + ".wav"
  mod_prog = "../" + PROG + "_" + mod
  name_sample = OGG_EXAMPLES + sample

  subprocess.call([mod_prog + " " + name_sample + " -o " + gen_sample], shell=True)

'''
  Permet de comparer un echantillon generer par notre programme avec une de base
'''
def compare_sample(sample,mod):
  temoin = WAV_EXAMPLES + sample + ".wav"
  gen_sample = DIR_GENERATED_SAMPLE + mod + "_" + sample + ".wav"
  log = open(LOG_FILE, "a+")
  
  log.write("----- %s -----\n" % sample)
  subprocess.call([PROG_TEST + " " + temoin + " " + gen_sample], shell=True, stdout=log)
  log.write("\n")
  log.close()

'''
  Permet de demander le test d'un module
'''
def test_module(id_mod):
  generate_prog(MODULES[id_mod-1])
  for sample in os.listdir(OGG_EXAMPLES):
    print("\n---------- %s ------------" % sample)
    generate_sample(sample, MODULES[id_mod-1])
    compare_sample(sample, MODULES[id_mod-1])

''' 
  Permet de tester tout les modules
'''
def test_all_modules():
  for id_mod in xrange(len(MODULES)):
    test_module(id_mod+1)

###########################
#          MAIN           #
###########################

print_header()

#Remove previous log file
if os.path.isfile(LOG_FILE):
  os.remove(LOG_FILE)

shutil.rmtree(DIR_GENERATED_SAMPLE)
#Test if generated samples dir is created
if not os.path.exists(DIR_GENERATED_SAMPLE):
    os.makedirs(DIR_GENERATED_SAMPLE)


res = int(display_menu())

if res == 0:
  test_all_modules()
elif res in xrange(len(MODULES)+1):
  test_module(res)
else:
  print("Mauvais choix. Exit")

#Make clean
subprocess.call(["make clean -C ../"], shell=True)
