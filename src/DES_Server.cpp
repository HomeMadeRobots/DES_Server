#include "DES_Server.h"


/**************************************************************************************************/
/* Constructor */
DES_Server::DES_Server( void )
{
	this->Initialize_Client_Id();
	this->Initialize_Priority();
}
/**************************************************************************************************/


/**************************************************************************************************/
/* Component_Type_Operations */
/**************************************************************************************************/
void DES_Server::Cyclic( void )
{
	DES_Client_Manager* client_manager = Get_Priority_Client_Manager();
	E_DES_CLIENT_MANAGER_STATE client_manager_state;
	bool operation_finished = false;
	
	if( client_manager != nullptr )
	{
		client_manager_state = client_manager->Get_State();
		switch( client_manager_state )
		{
			case FREE :
				/* Shoul not be possible */
				break;
				
			case TREATING_KEY :
				operation_finished = client_manager->Compute_Sub_Key();
				break;
				
			case IDLE :
				/* Should not be possible */
				break;
				
			case TREATING_MESSAGE :
				operation_finished = client_manager->Treat_Message();
				break;
				
			default :
				/* Should not be possible */
				break;
		}
		
		if( true == operation_finished )
		{
			this->Shift_Priority();
		}
		
	}
}
/**************************************************************************************************/


/**************************************************************************************************/
/* Event reception points accessors */
/**************************************************************************************************/


/**************************************************************************************************/
/* Prodided port accessors */
/**************************************************************************************************/
i_Symmetric_Cryptography_Services* DES_Server::Get_Port__Cryptography_Services( void )
{
	return (i_Symmetric_Cryptography_Services*)(this);
}
/**************************************************************************************************/


/**************************************************************************************************/
/* Private methods */
/**************************************************************************************************/
DES_Client_Manager* DES_Server::Get_Free_Client_Manager( void )
{
	uint8_t free_idx = 0;
	DES_Client_Manager* free_mgr = nullptr;
	while( free_idx != MAX_DES_SERVER_NUMBER_OF_CLIENTS && free_mgr == nullptr )
	{
		if( (this->Client_Manager_List[free_idx]).Get_State() == FREE )
		{
			free_mgr = &(this->Client_Manager_List[free_idx]);
		}
		else
		{
			free_idx++;
		}
	}
	return free_mgr;
}
/*------------------------------------------------------------------------------------------------*/
void DES_Server::Initialize_Client_Id( void )
{
	uint8_t client_idx = 0;
	for( client_idx = 0 ; client_idx < MAX_DES_SERVER_NUMBER_OF_CLIENTS ; client_idx++ )
	{
		(this->Client_Manager_List[client_idx]).Client_Id = client_idx + 1;
	}
}
/*------------------------------------------------------------------------------------------------*/
DES_Client_Manager* DES_Server::Get_Client_Manager_By_Id( uint8_t id )
{
	DES_Client_Manager* client_mgr = nullptr;
	uint8_t client_idx = 0;
	/* Test id validity */
	if( 0 != id )
	{
		/* Parse Client_Manager_List */
		while( client_idx != MAX_DES_SERVER_NUMBER_OF_CLIENTS && client_mgr == nullptr )
		{
			/* Test the id of the Client_Manager */
			if( (this->Client_Manager_List[client_idx]).Client_Id == id )
			{
				/* The id of the Client_Manager is the serched one */
				/* Check the state of the manager */
				if( (this->Client_Manager_List[client_idx]).Get_State() != FREE )
				{
					/* Manager is not FREE : the right one is found ! */ 
					client_mgr = &(this->Client_Manager_List[client_idx]);
				}
				else
				{
					/* Manager is FREE, it means the id has not been allocated to a client : prb */
					client_idx = MAX_DES_SERVER_NUMBER_OF_CLIENTS;
				}
			}
			else
			{
				/* Go to next Client_Manager */
				client_idx++;
			}
		}
	}
	return client_mgr;
}
/*------------------------------------------------------------------------------------------------*/
void DES_Server::Initialize_Priority( void )
{
	uint8_t client_idx = 0;
	for( client_idx = 0 ; client_idx < MAX_DES_SERVER_NUMBER_OF_CLIENTS ; client_idx++ )
	{
		(this->Priority_Client_Manager_List[client_idx]) = nullptr;
	}
}
/*------------------------------------------------------------------------------------------------*/
void DES_Server::Add_To_Treatment_Queue( DES_Client_Manager* manager )
{
	DES_Client_Manager* current_mgr = nullptr;
	uint8_t current_mgr_idx = 0;
	current_mgr = this->Priority_Client_Manager_List[0];
	while( current_mgr != nullptr )
	{
		current_mgr_idx++;
		current_mgr = this->Priority_Client_Manager_List[current_mgr_idx];
	}
	this->Priority_Client_Manager_List[current_mgr_idx] = manager;
}
/*------------------------------------------------------------------------------------------------*/
DES_Client_Manager* DES_Server::Get_Priority_Client_Manager( void )
{
	return this->Priority_Client_Manager_List[0];
}
/*------------------------------------------------------------------------------------------------*/
void DES_Server::Shift_Priority( void )
{
	uint8_t mgr_idx = 0;
	for( mgr_idx = 0 ; mgr_idx < MAX_DES_SERVER_NUMBER_OF_CLIENTS - 1 ; mgr_idx++ )
	{
		this->Priority_Client_Manager_List[mgr_idx] = this->Priority_Client_Manager_List[mgr_idx+1];
	}
    this->Priority_Client_Manager_List[MAX_DES_SERVER_NUMBER_OF_CLIENTS-1] = nullptr;
}
/*------------------------------------------------------------------------------------------------*/
void DES_Server::Remove_From_Priority_List( DES_Client_Manager* mgr )
{
	uint8_t remove_mgr_idx = 0;
	DES_Client_Manager* current_mgr = this->Priority_Client_Manager_List[remove_mgr_idx];
	
	/* Found the manager in the list */
	while( remove_mgr_idx < MAX_DES_SERVER_NUMBER_OF_CLIENTS && current_mgr != mgr )
	{
		remove_mgr_idx++;
		current_mgr = this->Priority_Client_Manager_List[remove_mgr_idx];
	}
	
	/* If founded, remove the manager from the queue */
	if( current_mgr == mgr )
	{
		this->Priority_Client_Manager_List[remove_mgr_idx] = nullptr;
	}
	
	/* Shift less priority manager */
	uint8_t mgr_idx;
	for( mgr_idx = remove_mgr_idx ; mgr_idx < MAX_DES_SERVER_NUMBER_OF_CLIENTS - 1 ; mgr_idx++ )
	{
		this->Priority_Client_Manager_List[mgr_idx] = this->Priority_Client_Manager_List[mgr_idx+1];
	}
    this->Priority_Client_Manager_List[MAX_DES_SERVER_NUMBER_OF_CLIENTS-1] = nullptr;
}
/**************************************************************************************************/


/**************************************************************************************************/
/* Provided operations */
/**************************************************************************************************/
bool DES_Server::Open_Session( 
	Asynchronous_Operation_Manager* async_op_mgr, 
	const uint8_t* key,
	E_SESSION_REQUEST_STATUS* request_status,
	uint8_t* client_id )
{
	bool op_accepted = true;
	DES_Client_Manager* free_client_manager;

	free_client_manager = this->Get_Free_Client_Manager();
	
	if( nullptr==free_client_manager )
	{
		op_accepted = false;
	}
	else
	{
		free_client_manager->Register_Open_Session_Arguments_Ref(
			key,
			request_status,
			client_id,
			async_op_mgr );
			
		this->Add_To_Treatment_Queue( free_client_manager );
	}
	
	return op_accepted;
}
/*------------------------------------------------------------------------------------------------*/
void DES_Server::Close_Session( uint8_t client_id )
{
	DES_Client_Manager* client_mng = nullptr;
	client_mng = Get_Client_Manager_By_Id( client_id );
	if( nullptr != client_mng )
	{
		this->Remove_From_Priority_List( client_mng );
		client_mng->Free_Manager();
	}
}
/*------------------------------------------------------------------------------------------------*/
void DES_Server::Get_Session_Status( uint8_t client_id, E_SESSION_STATUS* status )
{
	DES_Client_Manager* client_mng = nullptr;
	client_mng = Get_Client_Manager_By_Id( client_id );
	
	if( nullptr == client_mng )
	{
		*status = SESSION_STATUS_UNKNOWN_CLIENT;
	}
}
/*------------------------------------------------------------------------------------------------*/
bool DES_Server::Treat_Message(
	Asynchronous_Operation_Manager* async_op_mgr, 
	uint8_t client_id,
	const uint8_t* input_message,
	uint16_t message_length,
	E_CRYPTO_ACTION action,
	uint8_t* output_message)
{
	bool op_accepted = true;
	
	DES_Client_Manager* client_mgr = nullptr;
	client_mgr = Get_Client_Manager_By_Id( client_id );
	
	if( nullptr == client_mgr )
	{
		op_accepted = false;
	}
	else
	{
		if( IDLE != client_mgr->Get_State() )
		{
			op_accepted = false;
		}
		else
		{
			client_mgr->Register_Treat_Message_Arguments_Ref(
				async_op_mgr,
				input_message,
				message_length,
				action,
				output_message );
			this->Add_To_Treatment_Queue( client_mgr );
		}
	}
	
	return op_accepted;
}
/**************************************************************************************************/


/**************************************************************************************************/
/* Received events */
/**************************************************************************************************/