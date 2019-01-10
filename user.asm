
/*********************************************************************************************
* Fichero:		user.asm
* Autores:		Yasmina Albero e Iv�n Escu�n
* Descrip:		paso a modo usuario
* Version: 1.0
*********************************************************************************************/

#################################################################################################################
#Funcion modo_usuario
#################################################################################################################

.section .text
.global modo_usuario
modo_usuario:
	.equ 	UserStack,	0xc7ff000

	.equ MODEMASK,0x1f /* Para selecci�n de M[4:0] */
	.equ USERMODE, 0x10 /* C�digo de modo Undef */
	mrs r0,cpsr /* Llevamos el registro de estado a r0 */
	bic r0,r0,#MODEMASK /* Borramos los bits de modo de r0 */
	orr r1,r0,#USERMODE /* A�adimos el c�digo del modo USUARIO y copiamos en r1 */
	msr cpsr_cxsf,r1 /* Escribimos el resultado en el registro de estado, cambiando de �ste los bits de del
						campo de control, de extension, de estado y los de flag. */
	ldr sp,=UserStack /* Una vez en modo Undef copiamos la direcci�n de comienzo de la pila */

	bx lr
