 /*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 2 as
 * published by the Free Software Foundation.  You may not use, modify or
 * distribute this program under any other version of the GNU General
 * Public License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * Copyright (C) 2014-2019 Cisco and/or its affiliates. All rights reserved.
 * Copyright (C) 2011-2013 Sourcefire, Inc.
 *
 * Author: Chamara Devanarayana
 *
 *
 * Dynamic preprocessor for the IEC104 protocol
 *
 */


#include <string.h>
#include <stdint.h>




#include "iec104-app.h"





/* Append a IEC104 Transport segment to the reassembly buffer.

   Returns:
    IEC104_OK:    Segment queued successfully.
    IEC104_FAIL:  Data copy failed. Segment did not fit in reassembly buffer.
 */
static int IEC104QueueSegment(iec104_reassembly_data_t *rdata, char *buf, uint16_t buflen)
{
	if (rdata == NULL || buf == NULL)
		return IEC104_FAIL;

	/* At first I was afraid, but we checked for DNP3_MAX_TRANSPORT_LEN earlier. */
	if (buflen + rdata->buflen > IEC60870_5_104_MAX_ASDU_LENGTH + IEC60870_5_104_APCI_LENGTH )
		return IEC104_FAIL;

	memcpy((rdata->buffer + rdata->buflen), buf, (size_t) buflen);

	rdata->buflen += buflen;
	return IEC104_OK;
}

/* Reset a IEC104 reassembly buffer */
static void IEC104ReassemblyReset(iec104_reassembly_data_t *rdata)
{
	rdata->buflen = 0;
	rdata->last_seq = 0;
	rdata->asduAddress = 0;;
	rdata->numObj=0;
	rdata->orgAddress=0;
	rdata->startingAddress=0;
	rdata->typeID = 0;

}
//M_SP_NA_1 = 1,
//   M_SP_TA_1 = 2,
//   M_DP_NA_1 = 3,
//   M_DP_TA_1 = 4,
//   M_ST_NA_1 = 5,
//   M_ST_TA_1 = 6,
//   M_BO_NA_1 = 7,
//   M_BO_TA_1 = 8,
//   M_ME_NA_1 = 9,
//   M_ME_TA_1 = 10,
//   M_ME_NB_1 = 11,
//   M_ME_TB_1 = 12,
//   M_ME_NC_1 = 13,
//   M_ME_TC_1 = 14,
//   M_IT_NA_1 = 15,
//   M_IT_TA_1 = 16,
//   M_EP_TA_1 = 17,
//   M_EP_TB_1 = 18,
//   M_EP_TC_1 = 19,
//   M_PS_NA_1 = 20,
//   M_ME_ND_1 = 21,



static int modifyData(iec104_config_t *config, iec104_session_data_t *session,uint8_t * pdu_start, uint16_t pdu_length)
{
int modified = 0;
for(int index = 0 ; index<config->numAlteredVal;index++)
	{

		if((config->values_to_alter[index]).typeID == session->asdu_header.type && ((config->values_to_alter[index]).asduAddress)==session->asdu_header.asduAddress )
		{
			int32_t key = (config->values_to_alter[index]).infObjAddress;
			iec104_Object_header_t *objOfConcern = (session->hash).find(key)->second;
					 if(objOfConcern)
					 {switch(session->asdu_header.type)
						{
					 case(M_SP_NA_1):
							 memcpy((pdu_start+objOfConcern->dataOffsetFromStart),&((config->values_to_alter[index]).integer_value),LEN_M_SP_NA_1);
					 	 	 break;
					 case(M_SP_TA_1):
					 							 memcpy((pdu_start+objOfConcern->dataOffsetFromStart),&((config->values_to_alter[index]).integer_value),LEN_M_SP_TA_1-CP24Time_LEN);
					 					 	 	 break;
					 case(M_DP_NA_1):
					 							 memcpy((pdu_start+objOfConcern->dataOffsetFromStart),&((config->values_to_alter[index]).integer_value),LEN_M_DP_NA_1);
					 					 	 	 break;
					 case(M_DP_TA_1):
					 							 memcpy((pdu_start+objOfConcern->dataOffsetFromStart),&((config->values_to_alter[index]).integer_value),LEN_M_DP_TA_1-CP24Time_LEN);
					 					 	 	 break;
					 case(M_ST_NA_1):
					 							 memcpy((pdu_start+objOfConcern->dataOffsetFromStart),&((config->values_to_alter[index]).integer_value),LEN_M_ST_NA_1-QDP_LEN);
					 					 	 	 break;
					 case(M_ST_TA_1):
					 							 memcpy((pdu_start+objOfConcern->dataOffsetFromStart),&((config->values_to_alter[index]).integer_value),LEN_M_ST_TA_1-QDP_LEN-CP24Time_LEN);
					 					 	 	 break;
					 case(M_BO_NA_1):
					 							 memcpy((pdu_start+objOfConcern->dataOffsetFromStart),&((config->values_to_alter[index]).integer_value),LEN_M_BO_NA_1-QDP_LEN);
					 					 	 	 break;
					 case(M_BO_TA_1):
					 							 memcpy((pdu_start+objOfConcern->dataOffsetFromStart),&((config->values_to_alter[index]).integer_value),LEN_M_BO_TA_1-QDP_LEN-CP24Time_LEN);
					 					 	 	 break;

					 case(M_ME_NC_1):
					 							 memcpy((pdu_start+objOfConcern->dataOffsetFromStart),&((config->values_to_alter[index]).floating_point_val),LEN_M_ME_NC_1-QDP_LEN);
					 					 	 	 break;

					 case(C_SC_NA_1):
								{
						 uint8_t sco = 0;
						 uint8_t cause = 0;
						 memcpy(&sco,(pdu_start+objOfConcern->dataOffsetFromStart),SCO_LEN);

						 memcpy(&cause,(pdu_start+objOfConcern->dataOffsetFromStart-7),SCO_LEN);
						 if(cause==6)
						 {

							 (config->values_to_alter[index]).integer_value_old = sco;

							 if ((config->values_to_alter[index]).integer_value ==0)
							 {
								 sco&=0xFE;
							 }
							 else
								 sco |=0x01;
							 memcpy((pdu_start+objOfConcern->dataOffsetFromStart),&(sco),SCO_LEN);

						 }
						 else
						 {

							 memcpy((pdu_start+objOfConcern->dataOffsetFromStart),&((config->values_to_alter[index]).integer_value_old),SCO_LEN);
						 }
								}

					 break;

					 case(C_DC_NA_1):
												 memcpy((pdu_start+objOfConcern->dataOffsetFromStart),&((config->values_to_alter[index]).integer_value),DCO_LEN);
												 break;
					 case(C_RC_NA_1):
												 memcpy((pdu_start+objOfConcern->dataOffsetFromStart),&((config->values_to_alter[index]).integer_value),RCO_LEN);
												 break;
					 case(C_SE_NA_1):
					 							 memcpy((pdu_start+objOfConcern->dataOffsetFromStart),&((config->values_to_alter[index]).integer_value),NVA_LEN);
					 							 break;
					 case(C_SE_NB_1):
												 memcpy((pdu_start+objOfConcern->dataOffsetFromStart),&((config->values_to_alter[index]).integer_value),SVA_LEN);
												 break;
					 case(C_SE_NC_1):
						{
						 uint8_t cause = 0;
						 float sco = 0.0;

						 						 	memcpy(&cause,(pdu_start+objOfConcern->dataOffsetFromStart-7),SCO_LEN);
						 						 	if(cause==6)
						 						 	{
						 						 		memcpy(&sco,(pdu_start+objOfConcern->dataOffsetFromStart),sizeof(float));
						 						 		(config->values_to_alter[index]).floating_point_val_old = sco;

						 						 		memcpy((pdu_start+objOfConcern->dataOffsetFromStart),&((config->values_to_alter[index]).floating_point_val),IEEE_STD_754_LEN);
						 							}
						 						 	else
						 						 	{
						 						 		memcpy((pdu_start+objOfConcern->dataOffsetFromStart),&((config->values_to_alter[index]).floating_point_val_old),IEEE_STD_754_LEN);

						 						 	}


						}
						 break;
					 case(C_BO_NA_1):
												 memcpy((pdu_start+objOfConcern->dataOffsetFromStart),&((config->values_to_alter[index]).integer_value),BSI_LEN);
												 break;



					 default:
						 break;

						}
					 modified = 1;

					 }

					 uint16_t byteNumber;

		}
	}


	return modified;
}
/* Main IEC104 Reassembly function. Moved here to avoid circular dependency between
   spp_iec104 and iec104_reassembly. */
int IEC104FullReassembly(iec104_config_t *config, iec104_session_data_t *session,  ns3::Ptr<const ns3::Packet> packet, uint8_t *pdu_start, uint16_t pdu_length)
{

	iec104_header_t *link;
	iec104_reassembly_data_t *rdata;
	uint16_t offset = 0;
	static uint8_t typeLengths[128];
	typeLengths[0]= LEN_M_SP_NA_1;
	typeLengths[1]= LEN_M_SP_TA_1;
	typeLengths[2]= LEN_M_DP_NA_1  ;
	typeLengths[3]= LEN_M_DP_TA_1 ;
	typeLengths[4]= LEN_M_ST_NA_1  ;
	typeLengths[5]= LEN_M_ST_TA_1  ;
	typeLengths[6]= LEN_M_BO_NA_1  ;
	typeLengths[7]= LEN_M_BO_TA_1  ;
	typeLengths[8]= LEN_M_ME_NA_1  ;
	typeLengths[9]= LEN_M_ME_TA_1  ;
	typeLengths[10]= LEN_M_ME_NB_1  ;
	typeLengths[11]= LEN_M_ME_TB_1  ;
	typeLengths[12]= LEN_M_ME_NC_1 ;
	typeLengths[13]= LEN_M_ME_TC_1 ;
	typeLengths[14]= LEN_M_IT_NA_1  ;
	typeLengths[15]= LEN_M_IT_TA_1 ;
	typeLengths[16]= LEN_M_EP_TA_1  ;
	typeLengths[17]= LEN_M_EP_TB_1  ;
	typeLengths[18]= LEN_M_EP_TC_1  ;
	typeLengths[19]= LEN_M_PS_NA_1  ;
	typeLengths[20]= LEN_M_ME_ND_1 ;
	typeLengths[21]= LEN_M_ME_ND_1 ;


	typeLengths[30]= LEN_M_SP_TB_1 ;
	typeLengths[31]= LEN_M_DP_TB_1 ;
	typeLengths[32]= LEN_M_ST_TB_1 ;
	typeLengths[33]= LEN_M_BO_TB_1  ;
	typeLengths[34]= LEN_M_ME_TD_1 ;
	typeLengths[35]= LEN_M_ME_TE_1  ;
	typeLengths[36]= LEN_M_ME_TF_1  ;
	typeLengths[37]= LEN_M_IT_TB_1  ;
	typeLengths[38]= LEN_M_EP_TD_1  ;
	typeLengths[39]= LEN_M_EP_TE_1 ;

	typeLengths[40]= LEN_M_EP_TF_1;
	//typeLengths[41]= LEN_S_IT_TC_1 ;
	typeLengths[45]= LEN_C_SC_NA_1 ;
	typeLengths[46]= LEN_C_DC_NA_1 ;
	typeLengths[47]= LEN_C_RC_NA_1  ;
	typeLengths[48]= LEN_C_SE_NA_1 ;
	typeLengths[49]= LEN_C_SE_NB_1 ;

	typeLengths[50]= LEN_C_SE_NC_1;
	typeLengths[51]= LEN_C_BO_NA_1 ;
	typeLengths[58]= LEN_C_SC_NA_1+7 ;
	typeLengths[59]= LEN_C_DC_NA_1+7 ;
	typeLengths[60]= LEN_C_RC_NA_1+7  ;
	typeLengths[61]= LEN_C_SE_NA_1+7 ;
	typeLengths[62]= LEN_C_SE_NB_1+7 ;
	typeLengths[63]= LEN_C_SE_NC_1+7  ;
	typeLengths[64]= LEN_C_BO_NA_1+7 ;

	if (pdu_length < (sizeof(iec104_header_t) ))
		return IEC104_FAIL;

	if ( pdu_length > IEC60870_5_104_MAX_ASDU_LENGTH + IEC60870_5_104_APCI_LENGTH )
		// this means PAF aborted - not DNP3
		return IEC104_FAIL;

	/* Step 1: Decode header and skip to data */
	session->apci.start = *pdu_start;
	offset++;
	session->apci.len = *(pdu_start+offset);
	offset++;
	//printf("Packet Arrived %d \n",(*(pdu_start+offset))& 0x3);


	if((*(pdu_start+offset) & 0x1)==0)
	{
		uint8_t temp[2];
		memcpy(temp,(pdu_start+offset),2);
		offset+=2;
		session->apci.format = 0; //I-Format
		memcpy(&(session->apci.sendSeqNo),temp,2);
		session->apci.sendSeqNo = (session->apci.sendSeqNo)>>1;
		//session->apci.sendSeqNo = ntohs(session->apci.sendSeqNo);
		memcpy(temp,(pdu_start+offset),2);
		offset+=2;
		memcpy(&(session->apci.recvSeqNo),temp,2);
		session->apci.recvSeqNo = (session->apci.recvSeqNo)>>1;

		memcpy(&(session->asdu_header.type),(pdu_start+offset),1);
		offset++;
		memcpy(&(session->asdu_header.numObjects),(pdu_start+offset),1);
		offset++;
		session->asdu_header.sq = (session->asdu_header.numObjects)>>7 ;
		(session->asdu_header.numObjects) = (session->asdu_header.numObjects) & 0x7F;
		memcpy(&(session->asdu_header.cot),(pdu_start+offset),1);
		offset++;
		session->asdu_header.testBit = (session->asdu_header.cot)>>7;
		session->asdu_header.positiveOrNegative = ((session->asdu_header.cot) & 0x40 )>>6;
		session->asdu_header.cot = static_cast<IEC104_CauseOfTransmission>((session->asdu_header.cot)& 0x3F);
		memcpy(&(session->asdu_header.orgAddress),(pdu_start+offset),1);
		offset++;
		memcpy(&(session->asdu_header.asduAddress),(pdu_start+offset),2);
		offset+=2;
		iec104_Object_header_t * obj = (iec104_Object_header_t * ) malloc(sizeof (iec104_Object_header_t)*session->asdu_header.numObjects);
		uint32_t * objAddressKey = (uint32_t *)malloc(sizeof(uint32_t)*session->asdu_header.numObjects);
		uint32_t *tempobjAddressKey = objAddressKey;
		int ObjOffset=0;
		int indexObj=0;
		for(iec104_Object_header_t *cur = obj;indexObj<session->asdu_header.numObjects;cur++,indexObj++)
		{
			if(cur==obj ||session->asdu_header.sq==0 )
			{
				cur->informationObjAddress = 0;
			memcpy(&(cur->informationObjAddress),(pdu_start+offset),3);

			printf("Seq: %d Key: %u Type %u\n",session->apci.sendSeqNo,cur->informationObjAddress,session->asdu_header.type);
			//cur->informationObjAddress = ntohl(cur->informationObjAddress);
			offset+=3;
			cur->dataOffsetFromStart = offset;
			memcpy(&(cur->informationElements),(pdu_start+offset),typeLengths[session->asdu_header.type-1]);
			}

			else
			{
				cur->informationObjAddress = obj->informationObjAddress+ObjOffset;
				cur->dataOffsetFromStart = obj->dataOffsetFromStart+(ObjOffset*typeLengths[session->asdu_header.type-1]);
				memcpy(&(cur->informationElements),(pdu_start+cur->dataOffsetFromStart),typeLengths[session->asdu_header.type-1]);
			}
			printf("IOA: %d Data offset: %d Value %d\n",cur->informationObjAddress,cur->dataOffsetFromStart,cur->informationElements);

			offset+=typeLengths[session->asdu_header.type];
			*tempobjAddressKey = cur->informationObjAddress;

			cur->infElementBytesUsed = typeLengths[session->asdu_header.type];


			session->hash.insert({*tempobjAddressKey,cur});
			tempobjAddressKey++;
			ObjOffset++;
		}

	}

//	guint length;
//	uint32_t** keys = (uint32_t**)g_hash_table_get_keys_as_array (session->hash,&length);
//	       if(length!=0){
//	    	   uint32_t ** temp = keys;
//	        for(int in = 0;in<length;in++)
//	        {
//	        	printf("Keys: %d length %d\n",*temp[in],length);
//	        }
//	       }
	else if((*(pdu_start+offset) & 0x3)==1)
		{
		uint8_t temp[2];

				offset+=2;
				session->apci.format = 1; //S-Format

				session->apci.sendSeqNo = 0;
				//session->apci.sendSeqNo = ntohs(session->apci.sendSeqNo);
				memcpy(temp,(pdu_start+offset),2);
				memcpy(&(session->apci.recvSeqNo),temp,2);
				session->apci.recvSeqNo = (session->apci.recvSeqNo)>>1;
		}
	else if((*(pdu_start+offset) & 0x3)==3)
		{
		session->apci.format = 2; //U-Format
		session->apci.testFrame = (*(pdu_start+offset) & 0xC0)>>6;
		session->apci.stopDataTransmission = (*(pdu_start+offset) & 0x03)>>4;
		session->apci.startDataTransmission = (*(pdu_start+offset) & 0x0C)>>2;
		offset+=4;
		}









	if(session->apci.format == 0)
	{
		modifyData(config,session,pdu_start, pdu_length);

	}


	return IEC104_OK;
}

