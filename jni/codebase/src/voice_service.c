#include <stdio.h>
#include <string.h>
#include "qmi.h"
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "voice_service_common_v02.h"
#include "voice_service_v02.h"

#define QMI_MSG_DEFAULT_TIMEOUT 5000

/* Global variables */
static int qmi_handle = QMI_INVALID_CLIENT_HANDLE;
static qmi_client_type voice_client_handle;
static qmi_idl_service_object_type voice_service_obj;

static void voice_unsol_ind(qmi_client_type user_handle, unsigned long msg_id,
		unsigned char *ind_buf, int ind_buf_len, void *ind_cb_data);
static void qmi_response_callback(qmi_client_type user_handle,
		unsigned long msg_id, void *resp_c_struct, int resp_c_struct_len,
		void *resp_cb_data, qmi_client_error_type transp_err);

int main(int argc, const char ** argv) {
	qmi_client_error_type rc;

	/* Initialize the qmi datastructure(Once per process ) */
	qmi_handle = qmi_init(NULL, NULL);
	if (qmi_handle < 0) {
		printf("Error: Qmi message library not initialized\n");
		return qmi_handle;
	}

	voice_service_obj = voice_get_service_object_v02();

	if (voice_service_obj == NULL) {
		printf("Error: IDL version mismatch\n");
		return -1;
	}

	rc = qmi_client_init(QMI_PORT_RMNET_0, voice_service_obj, voice_unsol_ind,
			NULL, &voice_client_handle);

	if (rc != QMI_NO_ERR) {
		printf("Error: connection not Initialized...Error Code:%d\n", rc);
		rc = qmi_client_release(voice_client_handle);
		if (rc < 0) {
			printf("Release not successful \n");
		} else {
			printf("QMI client release successful \n");
		}
		return -1;
	} else {
		printf("Connection Initialized....User Handle:%x\n",
				(unsigned int) voice_client_handle);
	}
	/**
	 * register the indication we're interested in
	 */
	voice_indication_register_req_msg_v02 ind_reg_req;
	voice_indication_register_resp_msg_v02 ind_reg_resp;
	memset(&ind_reg_req, 0, sizeof(ind_reg_req));
	memset(&ind_reg_resp, 0, sizeof(ind_reg_resp));
	ind_reg_req.call_events_valid = 1;
	ind_reg_req.call_events = 0x01;
	rc = qmi_client_send_msg_sync(voice_client_handle,
			QMI_VOICE_INDICATION_REGISTER_REQ_V02, &ind_reg_req,
			sizeof(ind_reg_req), &ind_reg_resp, sizeof(ind_reg_resp),
			QMI_MSG_DEFAULT_TIMEOUT);
	if (rc != QMI_NO_ERR) {
		printf(
				"Sending QMI_VOICE_INDICATION_REGISTER_REQ_V02 returned error %d\n",
				(int) rc);
		return -1;
	} else {
		printf("QMI_VOICE_INDICATION_REGISTER_REQ_V02 response received\n");
	}

	char input[20];
	while (1) {
		printf("type exit to quit:\n");
		scanf("%s", input);
		if (!strcmp(input, "exit")) {
			break;
		} else {
			strcpy(input, "");
		}
	}

	rc = qmi_client_release(voice_client_handle);
	if (rc < 0) {
		printf("Release not successful \n");
	} else {
		printf("QMI client release successful \n");
	}
	if (qmi_handle >= 0) {
		qmi_release(qmi_handle);
	}
	printf("QMI release done.........\n");

	return 0;
}

static void qmi_response_callback(qmi_client_type user_handle,
		unsigned long msg_id, void *resp_c_struct, int resp_c_struct_len,
		void *resp_cb_data, qmi_client_error_type transp_err) {
	printf("response callback invoked for message ID %X, result code %d\n",
			msg_id, transp_err);
}

static void voice_unsol_ind(qmi_client_type user_handle, unsigned long msg_id,
		unsigned char *ind_buf, int ind_buf_len, void *ind_cb_data) {
	printf("indication callback invoked\n");
	qmi_client_error_type rc;
	switch (msg_id) {
	case QMI_VOICE_ALL_CALL_STATUS_IND_V02:
		printf("QMI_VOICE_ALL_CALL_STATUS_IND_V02 received\n");

		voice_all_call_status_ind_msg_v02 all_call_status;
		memset(&all_call_status, 0, sizeof(voice_all_call_status_ind_msg_v02));
		rc = qmi_client_message_decode(user_handle, QMI_IDL_INDICATION, msg_id,
				(const void*) ind_buf, ind_buf_len, (void*) &all_call_status,
				sizeof(voice_all_call_status_ind_msg_v02));
		uint32_t i;
		for (i = 0; i < all_call_status.call_info_len; i++) {
			printf(
					"call id %d, call state %d, call type %d, direction %d, mode %d\n",
					all_call_status.call_info[i].call_id,
					all_call_status.call_info[i].call_state,
					all_call_status.call_info[i].call_type,
					all_call_status.call_info[i].direction,
					all_call_status.call_info[i].mode);
			/*
			if (all_call_status.call_info[i].call_state
					== CALL_STATE_INCOMING_V02) {
				voice_answer_call_req_msg_v02 answer_req;
				voice_answer_call_resp_msg_v02 answer_resp;
				memset(&answer_req, 0, sizeof(answer_req));
				memset(&answer_resp, 0, sizeof(answer_resp));
				answer_req.call_id = all_call_status.call_info[i].call_id;
				rc = qmi_client_send_msg_sync(voice_client_handle,
						QMI_VOICE_ANSWER_CALL_REQ_V02, &answer_req,
						sizeof(answer_req), &answer_resp, sizeof(answer_resp),
						QMI_MSG_DEFAULT_TIMEOUT);
				if (rc != QMI_NO_ERR) {
					printf(
							"Sending QMI_VOICE_ANSWER_CALL_REQ_V02 returned error %d\n",
							(int) rc);
				} else {
					printf(
							"QMI_VOICE_ANSWER_CALL_RESP_V02 response received\n");
				}
			}*/
		}
		break;
	}
}
