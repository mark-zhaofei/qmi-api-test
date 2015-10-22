#include <stdio.h>

#include "qmi_idl_lib.h"
#include "network_access_service_v01.h"
#include "user_identity_module_v01.h"
#include "voice_service_v02.h"

int main(int argc, const char ** argv){
	qmi_idl_service_object_type idl_service_obj;
    int32_t idl_minor;
    for(idl_minor = 0;idl_minor <= 0xFF;idl_minor++){
    	idl_service_obj = nas_get_service_object_internal_v01(0x01, idl_minor, 0x06);
    	if(idl_service_obj != NULL){
    		printf("NAS IDL minor version found 0x%X\n", idl_minor);
    		break;
    	}
    }
    if(idl_minor > 0xFF){
    	printf("NAS IDL minor version exceeds 0xFF\n");
    }
    for(idl_minor = 0;idl_minor <= 0xFF;idl_minor++){
    	idl_service_obj = uim_get_service_object_internal_v01(0x01, idl_minor, 0x06);
    	if(idl_service_obj != NULL){
    		printf("UIM IDL minor version found 0x%X\n", idl_minor);
    		break;
    	}
    }
    if(idl_minor > 0xFF){
    	printf("UIM IDL minor version exceeds 0xFF\n");
    }
    for(idl_minor = 0;idl_minor <= 0xFF;idl_minor++){
    	idl_service_obj = voice_get_service_object_internal_v02(0x02, idl_minor, 0x06);
    	if(idl_service_obj != NULL){
    		printf("Voice IDL minor version found 0x%X\n", idl_minor);
    		break;
    	}
    }
    if(idl_minor > 0xFF){
    	printf("Voice IDL minor version exceeds 0xFF\n");
    }
    return 0;
}
