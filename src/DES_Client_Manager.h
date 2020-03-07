#ifndef DES_CLIENT_MANAGER_H
#define DES_CLIENT_MANAGER_H

#include "Asynchronous_Operation_Manager.h"
#include "Data_Encryption_Standard_Class.h"
#include "E_SESSION_REQUEST_STATUS.h"
#include "E_CRYPTO_ACTION.h"
#include "stdint.h"

typedef enum E_DES_CLIENT_MANAGER_STATE {
		FREE,
		TREATING_KEY,
		IDLE,
		TREATING_MESSAGE
	} E_DES_CLIENT_MANAGER_STATE;
	
class DES_Client_Manager {
public :	
	DES_Client_Manager( void );
	
	E_DES_CLIENT_MANAGER_STATE Get_State( void );
	
	void Free_Manager( void );
	
	void Register_Open_Session_Arguments_Ref(
		const uint8_t* key_ref,
		E_SESSION_REQUEST_STATUS* request_status_ref,
		uint8_t* client_id_ref,
		Asynchronous_Operation_Manager* key_op_mgr );
		
	void Register_Treat_Message_Arguments_Ref(
		Asynchronous_Operation_Manager* message_op_mgr,
		const uint8_t* input_message_ref,
		uint16_t message_length,
		E_CRYPTO_ACTION action,
		uint8_t* output_message_ref );
		
	bool Compute_Sub_Key( void );
	bool Treat_Message( void );
		
	uint8_t Client_Id;
	
private :
	E_DES_CLIENT_MANAGER_STATE State;

	/* Attributes for Open_Session operation */
	Asynchronous_Operation_Manager* Key_Op_Mgr;
	const uint8_t* Key_Ref;
	E_SESSION_REQUEST_STATUS* Request_Status_Ref;
	uint8_t* Client_Id_Ref;
	
	/* Attributes for Treat_Message operation */
	Asynchronous_Operation_Manager* Message_Op_Mgr;
	const uint8_t* Input_Message_Ref;
	uint16_t Input_Message_Length;
	E_CRYPTO_ACTION Crypto_Action;
	uint8_t* Output_Message_Ref;
    uint16_t Block_Idx;
    uint16_t Nb_Block_To_Treat;
	
	Data_Encryption_Standard_Class DES_Services;
};

#endif