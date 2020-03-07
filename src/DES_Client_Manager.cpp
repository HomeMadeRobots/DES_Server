#include "DES_Client_Manager.h"

/**************************************************************************************************/
DES_Client_Manager::DES_Client_Manager( void )
{
	this->State = FREE;
	this->Key_Op_Mgr = nullptr;
	this->Block_Idx = 0;
}
/*------------------------------------------------------------------------------------------------*/
E_DES_CLIENT_MANAGER_STATE DES_Client_Manager::Get_State( void )
{
	return this->State;
}
/*------------------------------------------------------------------------------------------------*/
void DES_Client_Manager::Register_Open_Session_Arguments_Ref(
	const uint8_t* key_ref,
	E_SESSION_REQUEST_STATUS* request_status_ref,
	uint8_t* client_id_ref,
	Asynchronous_Operation_Manager* key_op_mgr )
{
	this->State = TREATING_KEY;
	this->Key_Ref = key_ref;
	this->Key_Op_Mgr = key_op_mgr;
	this->Request_Status_Ref = request_status_ref;
	this->Client_Id_Ref = client_id_ref;
}
/*------------------------------------------------------------------------------------------------*/
void DES_Client_Manager::Register_Treat_Message_Arguments_Ref(
	Asynchronous_Operation_Manager* message_op_mgr,
	const uint8_t* input_message_ref,
	uint16_t message_length,
	E_CRYPTO_ACTION action,
	uint8_t* output_message_ref )
{
	this->State = TREATING_MESSAGE;
	this->Message_Op_Mgr = message_op_mgr;
	this->Input_Message_Ref = input_message_ref;
	this->Input_Message_Length = message_length;
	this->Crypto_Action = action;
	this->Output_Message_Ref = output_message_ref;
    this->Block_Idx = 0;
    this->Nb_Block_To_Treat = ((this->Input_Message_Length) + (this->Input_Message_Length)%8 ) / 8;
}
/*------------------------------------------------------------------------------------------------*/		
void DES_Client_Manager::Free_Manager( void )
{
	this->State = FREE;
}
/*------------------------------------------------------------------------------------------------*/
bool DES_Client_Manager::Compute_Sub_Key( void )
{
	bool is_key_valid = true;
	bool is_operation_finished = true;
	
	is_key_valid = this->DES_Services.Set_New_Key(this->Key_Ref);
	
	if( true==is_key_valid )
	{
		*(this->Client_Id_Ref) = this->Client_Id;
		*(this->Request_Status_Ref) = SESSION_OPENED;
		this->Key_Op_Mgr->Srv_Notify_Operation_Finished( true );
		this->State = IDLE;
	}
	else
	{
		*(this->Client_Id_Ref) = 0;
		*(this->Request_Status_Ref) = INVALID_KEY;
		this->Key_Op_Mgr->Srv_Notify_Operation_Finished( false );
		this->State = FREE;
	}
	return is_operation_finished;
}
/*------------------------------------------------------------------------------------------------*/
bool DES_Client_Manager::Treat_Message( void )
{
	bool is_operation_finished = false;
	
    const uint8_t* block_to_treat = nullptr;
    uint8_t* result_block = nullptr;

    block_to_treat = this->Input_Message_Ref + (this->Block_Idx)*8 ;
    result_block = this->Output_Message_Ref + (this->Block_Idx)*8 ;

    if(CIPHER==this->Crypto_Action)
    {
        this->DES_Services.Cipher_Message( block_to_treat, result_block );
    }
    else
    {
        this->DES_Services.Decipher_Message( block_to_treat, result_block );
    }
    
    this->Block_Idx++;
    if( this->Block_Idx == this->Nb_Block_To_Treat )
    {
		this->Message_Op_Mgr->Srv_Notify_Operation_Finished( true );
		this->State = IDLE;
        is_operation_finished = true;
    }

	return is_operation_finished;
}	
/**************************************************************************************************/