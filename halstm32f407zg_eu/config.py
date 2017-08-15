from Arm.Stm32 import *

env = Stm32f407xx()
env.setLinkfile( '/ld/stm32f407zg_min.ld' )
port = 'ARM_CM4F'
env.appendDefineFlags( [ 'HSE_VALUE=8000000' ] )

env.appendDefineFlags( [ 'MCUSH_STACK_SIZE=10240' ] )

USE_VFS=True
USE_VFS=False

if USE_VFS:
    env.appendDefineFlags( [ 'MCUSH_VFS=1', 'MCUSH_SPIFFS=1' ] )
    env.appendDefineFlags( [ 'MCUSH_ROMFS=1' ] )
    env.appendPath([ '/libspiffs' ])
    env.appendGlobSource([ '/libspiffs/*.c' ])



 


