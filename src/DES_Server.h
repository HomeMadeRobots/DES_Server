#ifndef DES_SERVER_H
#define DES_SERVER_H

#define MAX_DES_SERVER_NUMBER_OF_CLIENTS 4

/* Attributes */
#include "Data_Encryption_Standard_Class.h"
#include "DES_Client_Manager.h"


/* Realized interfaces */
#include "i_Symmetric_Cryptography_Services.h"


/* Needed interfaces */


/* Events */


/* Component_Type */
class DES_Server : i_Symmetric_Cryptography_Services {
public :
    /*--------------------------------------------------------------------------------------------*/
    /* Constructor */
    DES_Server( void );

    /*--------------------------------------------------------------------------------------------*/
    /* Component_Type_Operations */
	void Cyclic( void );
	
    /*--------------------------------------------------------------------------------------------*/
    /* Event reception points accessors */

    /*--------------------------------------------------------------------------------------------*/
    /* Provided port accessors */
	i_Symmetric_Cryptography_Services* Get_Port__Cryptography_Services( void );
	
	/*--------------------------------------------------------------------------------------------*/
    /* Provided operations */
	bool Open_Session( 
		Asynchronous_Operation_Manager* async_op_mgr, 
		const uint8_t* key,
		E_SESSION_REQUEST_STATUS* request_status,
		uint8_t* client_id ) override;
    void Close_Session( uint8_t client_id ) override;
	void Get_Session_Status( uint8_t client_id, E_SESSION_STATUS* status ) override;
    bool Treat_Message(
		Asynchronous_Operation_Manager* async_op_mgr, 
		uint8_t client_id,
		const uint8_t* input_message,
		uint16_t message_length,
		E_CRYPTO_ACTION action,
		uint8_t* output_message) override;
	
private :
    /*--------------------------------------------------------------------------------------------*/
    /* Private attributes */
	DES_Client_Manager Client_Manager_List[MAX_DES_SERVER_NUMBER_OF_CLIENTS];
	DES_Client_Manager* Priority_Client_Manager_List[MAX_DES_SERVER_NUMBER_OF_CLIENTS];

    /*--------------------------------------------------------------------------------------------*/
    /* Private methods */
	DES_Client_Manager* Get_Free_Client_Manager( void );
	
	void Initialize_Client_Id( void );
	DES_Client_Manager* Get_Client_Manager_By_Id( uint8_t id );
	
	void Initialize_Priority( void );
	void Add_To_Treatment_Queue( DES_Client_Manager* manager );
	DES_Client_Manager* Get_Priority_Client_Manager( void );
	void Shift_Priority( void );
	void Remove_From_Priority_List( DES_Client_Manager* mgr );
	
    /*--------------------------------------------------------------------------------------------*/
    /* Requirer_Ports */

    /*--------------------------------------------------------------------------------------------*/
    /* Provider ports */

    /*--------------------------------------------------------------------------------------------*/
    /* Sent events */

    /*--------------------------------------------------------------------------------------------*/
    /* Received events */
};

#endif