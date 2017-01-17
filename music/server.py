from pygame import mixer
import pygame

from time import sleep
mixer.pre_init(44100, -16, 2, 2048)
pygame.init()
mixer.music.load("0001.wav")
mixer.music.play()

while True:
    pass

