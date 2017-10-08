#include <e3iface-inventory.h>
#include <e3_init.h>
#include <e3-api-wrapper.h>
#include <e3_log.h>

struct e3iface_model_def model_defs[E3IFACE_MODEL_MAX_MODELS];
struct e3iface_role_def  role_defs[E3IFACE_ROLE_MAX_ROLES];




static int generic_singly_queue_pre_setup(struct E3Interface * pif)
{
	pif->hwiface_model=E3IFACE_MODEL_GENERIC_SINGLY_QUEUE;
	pif->nr_queues=1;
	return 0;
}
static int generic_singly_queue_port_config(struct E3Interface * pif,struct rte_eth_conf *port_config)
{
	RTE_SET_USED(pif);
	port_config->rxmode.mq_mode=ETH_MQ_RX_NONE;
	port_config->rxmode.max_rx_pkt_len=ETHER_MAX_LEN;
	port_config->rxmode.header_split=0;
	port_config->rxmode.hw_ip_checksum=1;
	port_config->rxmode.hw_vlan_filter=0;
	port_config->rxmode.hw_vlan_strip=0;/*here diable vlan stripping,
										if we need to strip vlan, 
										then call rte_eth_dev_set_vlan_offload() to do it*/
	port_config->rxmode.hw_vlan_extend=0;
	port_config->rxmode.jumbo_frame=0;
	port_config->rxmode.hw_strip_crc=1;
	port_config->rxmode.enable_scatter=0;
	port_config->rxmode.enable_lro=0;
	port_config->txmode.mq_mode=ETH_MQ_TX_NONE;
	
	return 0;
}





void e3iface_inventory_init(void)
{
	memset(model_defs,0x0,sizeof(model_defs));
	memset(role_defs,0x0,sizeof(role_defs));

	model_defs[E3IFACE_MODEL_GENERIC_SINGLY_QUEUE].is_set=1;
	model_defs[E3IFACE_MODEL_GENERIC_SINGLY_QUEUE].check_lsc=0;
	model_defs[E3IFACE_MODEL_GENERIC_SINGLY_QUEUE].lsc_iface_up=NULL;
	model_defs[E3IFACE_MODEL_GENERIC_SINGLY_QUEUE].lsc_iface_down=NULL;
	model_defs[E3IFACE_MODEL_GENERIC_SINGLY_QUEUE].queue_setup=generic_singly_queue_pre_setup;
	model_defs[E3IFACE_MODEL_GENERIC_SINGLY_QUEUE].port_setup=generic_singly_queue_port_config;
	
}

E3_init(e3iface_inventory_init,TASK_PTIORITY_LOW);






/*export C&Python API*/
e3_type create_e3iface(e3_type service,
		e3_type dev_params,
		e3_type model,
		e3_type role,
		e3_type pport)
{
	#define _(c) if(!(c)) goto error
	char * 		_dev_paarams=(char*)e3_type_to_uint8_t_ptr(dev_params);
	int    		_model=e3_type_to_uint8_t(model);
	int    		_role=e3_type_to_uint8_t(role);
	uint32_t *  _pport=(uint32_t*)e3_type_to_uint8_t_ptr(pport);
	struct E3Interface_ops ops;
	_((_model>=0)&&(_model<E3IFACE_MODEL_MAX_MODELS));
	_((_role>=0)&&(_role<E3IFACE_ROLE_MAX_ROLES));
	_(model_defs[_model].is_set);
	_(role_defs[_role].is_set);
	
	memset(&ops,0x0,sizeof(ops));
	ops.lsc_iface_up            =model_defs[_model].lsc_iface_up;
	ops.lsc_iface_down          =model_defs[_model].lsc_iface_down;
	ops.queue_setup             =model_defs[_model].queue_setup;
	ops.port_setup              =model_defs[_model].port_setup;
	ops.check_lsc               =model_defs[_model].check_lsc;
	
	ops.capability_check        =role_defs[_role].capability_check;
	ops.input_node_process_func =role_defs[_role].input_node_process_func;
	ops.output_node_process_func=role_defs[_role].output_node_process_func;
	ops.priv_size               =role_defs[_role].priv_size;
	ops.post_setup              =role_defs[_role].post_setup;
	memcpy(ops.edges,role_defs[_role].edges,sizeof(ops.edges));
	
	return create_e3iface_with_slowpath(_dev_paarams,&ops,(int*)_pport);
	#undef _
	error:
		return -1;
}
DECLARE_E3_API(e3iface_creation)={
	.api_name="create_e3iface",
	.api_desc="attach (v)dev as E3Interface",
	.api_callback_func=(api_callback_func)create_e3iface,
	.args_desc={
		{.type=e3_arg_type_uint8_t_ptr,.behavior=e3_arg_behavior_input,.len=128},
		{.type=e3_arg_type_uint8_t,.behavior=e3_arg_behavior_input,},
		{.type=e3_arg_type_uint8_t,.behavior=e3_arg_behavior_input,},
		{.type=e3_arg_type_uint8_t_ptr,.behavior=e3_arg_behavior_output,.len=4},
		{.type=e3_arg_type_none,}
	},
};


e3_type reclaim_e3iface(e3_type service,e3_type iface)
{
	uint16_t _iface=e3_type_to_uint16_t(iface);
	return release_e3iface_with_slowpath(_iface);
}
DECLARE_E3_API(e3iface_reclaim)={
	.api_name="reclaim_e3iface",
	.api_desc="reclaim the given e3iface as with its associated peer iface",
	.api_callback_func=(api_callback_func)reclaim_e3iface,
	.args_desc={
		{.type=e3_arg_type_uint16_t,.behavior=e3_arg_behavior_input,},
		{.type=e3_arg_type_none},
	},
};

e3_type update_e3fiace_status(e3_type service,e3_type iface,e3_type is_to_start)
{
	uint16_t _iface       =e3_type_to_uint16_t(iface);
	uint8_t  _is_to_start =!!e3_type_to_uint8_t(is_to_start);

	if(_is_to_start)
		start_e3interface_with_slow_path(_iface);
	else
		stop_e3interface_with_slow_path(_iface);
	return 0;
}
DECLARE_E3_API(e3iface_status_update)={
	.api_name="update_e3fiace_status",
	.api_desc="update the status of an e3iface",
	.api_callback_func=(api_callback_func)update_e3fiace_status,
	.args_desc={
		{.type=e3_arg_type_uint16_t,.behavior=e3_arg_behavior_input,},
		{.type=e3_arg_type_uint8_t,.behavior=e3_arg_behavior_input,},
		{.type=e3_arg_type_none,},
	},
	
};