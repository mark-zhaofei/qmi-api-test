#include <stdio.h>
#include <string.h>
#include "qmi.h"
#include "qmi_client.h"
#include "qmi_client_instance_defs.h"
#include "qmi_idl_lib.h"
#include "network_access_service_v01.h"

#define QMI_MSG_DEFAULT_TIMEOUT 5000

/* Global variables */
static int qmi_handle = QMI_INVALID_CLIENT_HANDLE;
static qmi_client_type nas_client_handle;
static qmi_idl_service_object_type nas_service_obj;

static void nas_unsol_ind (qmi_client_type user_handle, unsigned int msg_id, void *ind_buf, unsigned int ind_buf_len, void *ind_cb_data);
static void qmi_response_callback (qmi_client_type user_handle, unsigned long msg_id, void *resp_c_struct, int resp_c_struct_len, void *resp_cb_data, qmi_client_error_type transp_err);

int main(int argc, const char ** argv)
{
	qmi_client_error_type rc;
	int time_out = 4;

    /* Initialize the qmi datastructure(Once per process ) */
    qmi_handle = qmi_init(NULL,NULL);
    if (qmi_handle < 0)
    {
      printf("Error: Qmi message library not initialized\n");
      return qmi_handle;
    }

    nas_service_obj = nas_get_service_object_v01();
    qmi_client_os_params os_params;
	memset(&os_params, 0, sizeof(os_params));

    rc = qmi_client_init_instance(nas_service_obj, QMI_CLIENT_QMUX_RMNET_INSTANCE_0, nas_unsol_ind, NULL, &os_params, time_out, &nas_client_handle);

    //rc = qmi_client_init(QMI_PORT_RMNET_0, nas_service_obj, nas_unsol_ind, NULL, &nas_client_handle);

    if (rc != QMI_NO_ERR )
    {
         printf("Error: connection not Initialized...Error Code:%d\n",rc);
         rc = qmi_client_release(nas_client_handle);
         if (rc < 0 )
         {
            printf("Release not successful \n");
         }
         else
         {
            printf("QMI client release successful \n");
         }
    }
    else
    {
        printf("Connection Initialized....User Handle:%x\n", (unsigned int)nas_client_handle);
    }

    /*send QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01 request to QMI NAS service*/
	nas_get_system_selection_preference_req_msg_v01 evt_req;
	nas_get_system_selection_preference_resp_msg_v01 evt_resp;
	memset(&evt_req, 0, sizeof(evt_req));
	memset(&evt_resp, 0, sizeof(evt_resp));

	qmi_txn_handle txn;

	rc = qmi_client_send_msg_sync(nas_client_handle, QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01, &evt_req, sizeof(evt_req), &evt_resp, sizeof(evt_resp), QMI_MSG_DEFAULT_TIMEOUT);

	if (rc != QMI_NO_ERR)
    {
        printf("Sending NAS get system selection preference returned error %d\n", (int)rc);
    }
	else
	{
		printf("NAS get system selection preference response received\n");
	}

	rc = qmi_client_release(nas_client_handle);
    if (rc < 0 )
    {
       printf("Release not successful \n");
    }
    else
    {
       printf("QMI client release successful \n");
    }
    if (qmi_handle >= 0)
    {
      qmi_release(qmi_handle);
    }
    printf("QMI release done.........\n");

    return 0;
}

static void qmi_response_callback (qmi_client_type user_handle, unsigned long msg_id, void *resp_c_struct, int resp_c_struct_len, void *resp_cb_data, qmi_client_error_type transp_err)
{
	printf("response callback invoked for message ID %X, result code %d", msg_id, transp_err);
}

static void nas_unsol_ind (qmi_client_type user_handle, unsigned int msg_id, void *ind_buf, unsigned int ind_buf_len, void *ind_cb_data)
{
	printf("indication callback invoked\n");
}
