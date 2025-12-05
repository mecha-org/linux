/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * NXP Wireless LAN device driver: commands and events
 *
 * Copyright 2011-2024 NXP
 */

#ifndef _NXPWIFI_CMD_EVT_H_
#define _NXPWIFI_CMD_EVT_H_

struct nxpwifi_cmd_entry {
	u16 cmd_no;
	int (*prepare_cmd)(struct nxpwifi_private *priv,
			   struct host_cmd_ds_command *cmd,
			   u16 cmd_no, void *data_buf,
			   u16 cmd_action, u32 cmd_type);
	int (*cmd_resp)(struct nxpwifi_private *priv,
			struct host_cmd_ds_command *resp,
			u16 cmdresp_no,
			void *data_buf);
};

struct nxpwifi_evt_entry {
	u32 event_cause;
	int (*event_handler)(struct nxpwifi_private *priv);
};

static inline int
nxpwifi_cmd_fill_head_only(struct nxpwifi_private *priv,
			   struct host_cmd_ds_command *cmd,
			   u16 cmd_no, void *data_buf,
			   u16 cmd_action, u32 cmd_type)
{
	cmd->command = cpu_to_le16(cmd_no);
	cmd->size = cpu_to_le16(S_DS_GEN);

	return 0;
}

int nxpwifi_send_cmd(struct nxpwifi_private *priv, u16 cmd_no,
		     u16 cmd_action, u32 cmd_oid, void *data_buf, bool sync);
int nxpwifi_sta_prepare_cmd(struct nxpwifi_private *priv,
			    struct cmd_ctrl_node *cmd_node,
			    u16 cmd_action, u32 cmd_oid);
int nxpwifi_dnld_dt_cfgdata(struct nxpwifi_private *priv,
			    struct device_node *node, const char *prefix);
int nxpwifi_sta_init_cmd(struct nxpwifi_private *priv, u8 first_sta, bool init);
int nxpwifi_uap_prepare_cmd(struct nxpwifi_private *priv,
			    struct cmd_ctrl_node *cmd_node,
			    u16 cmd_action, u32 type);
int nxpwifi_set_secure_params(struct nxpwifi_private *priv,
			      struct nxpwifi_uap_bss_param *bss_config,
			      struct cfg80211_ap_settings *params);
void nxpwifi_set_ht_params(struct nxpwifi_private *priv,
			   struct nxpwifi_uap_bss_param *bss_cfg,
			   struct cfg80211_ap_settings *params);
void nxpwifi_set_vht_params(struct nxpwifi_private *priv,
			    struct nxpwifi_uap_bss_param *bss_cfg,
			    struct cfg80211_ap_settings *params);
void nxpwifi_set_tpc_params(struct nxpwifi_private *priv,
			    struct nxpwifi_uap_bss_param *bss_cfg,
			    struct cfg80211_ap_settings *params);
void nxpwifi_set_uap_rates(struct nxpwifi_uap_bss_param *bss_cfg,
			   struct cfg80211_ap_settings *params);
void nxpwifi_set_vht_width(struct nxpwifi_private *priv,
			   enum nl80211_chan_width width,
			   bool ap_11ac_disable);
bool nxpwifi_check_11ax_capability(struct nxpwifi_private *priv,
				   struct nxpwifi_uap_bss_param *bss_cfg,
				   struct cfg80211_ap_settings *params);
int nxpwifi_set_11ax_status(struct nxpwifi_private *priv,
			    struct nxpwifi_uap_bss_param *bss_cfg,
			    struct cfg80211_ap_settings *params);
void nxpwifi_set_sys_config_invalid_data(struct nxpwifi_uap_bss_param *config);
void nxpwifi_set_wmm_params(struct nxpwifi_private *priv,
			    struct nxpwifi_uap_bss_param *bss_cfg,
			    struct cfg80211_ap_settings *params);
void nxpwifi_config_uap_11d(struct nxpwifi_private *priv,
			    struct cfg80211_beacon_data *beacon_data);
void nxpwifi_uap_set_channel(struct nxpwifi_private *priv,
			     struct nxpwifi_uap_bss_param *bss_cfg,
			     struct cfg80211_chan_def chandef);
int nxpwifi_config_start_uap(struct nxpwifi_private *priv,
			     struct nxpwifi_uap_bss_param *bss_cfg);

int nxpwifi_process_event(struct nxpwifi_adapter *adapter);
int nxpwifi_process_sta_event(struct nxpwifi_private *priv);
int nxpwifi_process_uap_event(struct nxpwifi_private *priv);
void nxpwifi_reset_connect_state(struct nxpwifi_private *priv, u16 reason,
				 bool from_ap);
void nxpwifi_process_multi_chan_event(struct nxpwifi_private *priv,
				      struct sk_buff *event_skb);
void nxpwifi_process_tx_pause_event(struct nxpwifi_private *priv,
				    struct sk_buff *event);
void nxpwifi_bt_coex_wlan_param_update_event(struct nxpwifi_private *priv,
					     struct sk_buff *event_skb);

#endif /* !_NXPWIFI_CMD_EVT_H_ */
