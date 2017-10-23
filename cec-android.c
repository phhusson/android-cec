void init() {
	struct cec_log_addrs log_addrs;
	ioctl(fd, CEC_ADAP_G_LOG_ADDRS, &log_addrs);
	strcpy(log_addrs.osd_name, "phh");
	log_addrs.vendor_id = 0xeeaa;
	log_addrs.vendor_id = CEC_OP_CEC_VERSION_2_0;
	ioctl(fd, CEC_ADAP_S_LOG_ADDRS, &log_addrs);
}

//This code is WRONG, log_addr is set by Linux, but Android wants to set it.
//Android HAL knows "CecDeviceType" but never references it.
addLogicalAddress(CecLogicalAddress addr) generates (Result result) {
	struct cec_log_addrs log_addrs;
	int i;
	int n = 0;
	ioctl(fd, CEC_ADAP_G_LOG_ADDRS, &log_addrs);
	
	//First check if the logical address hasn't already been set
	for(int i=0; i < CEC_MAX_LOG_ADDRS; ++i) {
		int mask = 1 << i;
		if(log_addrs.log_addr_mask & mask) {
			if(log_addrs.log_addr[i] == addr)
				return SUCCESS;
		}
	}

	//Find the first place available in log_addr[]
	for(i=0; i < CEC_MAX_LOG_ADDRS; ++i) {
		int mask = 1 << i;
		if(!(log_addrs.log_addr_mask & mask)) {
			break;
		}
	}

	if(i == CEC_MAX_LOG_ADDRS) return FAILURE_INVALID_STATE;

	log_addrs.log_addr[i] = addr;
	log_addrs.log_addr_mask |= 1 << i;

	//Safety: Ensure num_log_addrs makes sense
	for(i=0; i < CEC_MAX_LOG_ADDRS; ++i) {
		int mask = 1 << i;
		if(log_addrs.log_addr_mask & mask)
			n++;
	}

	if(log_addrs.num_log_addrs != n-1) return FAILURE_INVALID_STATE;
	log_addrs.num_log_addrs++;

	ioctl(fd, CEC_ADAP_S_LOG_ADDRS, &log_addrs);

	result = SUCCESS;
}

clearLogicalAddress() {
	struct cec_log_addrs log_addrs;
	ioctl(fd, CEC_ADAP_G_LOG_ADDRS, &log_addrs);
	log_addrs.num_log_addrs = 0;
	log_addrs.log_addr_mask = 0;
	ioctl(fd, CEC_ADAP_S_LOG_ADDRS, &log_addrs);
}

setLanguage(string language) {
	struct cec_msg m;
	cec_msg_set_menu_language(&m, language);
	ioctl(fd, CEC_TRANSMIT, &m);
}

enableAudioReturnChannel(int portId, bool enable) {
	struct cec_msg m;
	cec_msg_initiate_arc(&m, 1);
	ioctl(fd, CEC_TRANSMIT, &m);
	//What to do with result?
}

setOption(OptionKey key, bool value) {
	if(key == WAKEUP) {
		//Uh... TV specific?
	}
	if(key == ENABLE_CEC) {
		//Uh... TV specific?
	}
	if(key == SYSTEM_CEC_CONTROL) {
		//TBD when we have any clue about low-power
	}
}

getPortInfo() generates(vec<HdmiPortInfo> infos) {
	infos[0]->type = OUTPUT;
	infos[0]->portId = 1;
	infos[0]->cecSupported = 1;
	infos[0]->arcSupported = 0;
	uint16_t physicalAddress;
	ioctl(fd, CEC_ADAP_G_PHYS_ADDR, &physicalAddress);
	infos[0]->physicalAddress = physicalAddress;
}

getVendorId() generates(uint32_t vendor_id) {
}

getCecVersion() generates (int32_t version) {
	hdmi_cec_2
}

sendMessage(CecMessage msg) generates (SendMessageResult result) {

}
