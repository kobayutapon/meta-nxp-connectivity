#!/bin/bash

#################################################################################################################################################
# This script is to start or stop Openthread Host applications on supported i.MX platforms
#
#	-> It can be configured to run Spi, DualPan and Uart
#
#	-> It uses "Dtb Gpio Naming" feature to detect the right gpiochip Number and Line, and SPI device number for both Spi and Dual modes
#
#	-> It always starts (without any verification !) either /usr/bin/ot-daemon (+ /usr/sbin/zb_mux in Dual mode) or /usr/sbin/otbr-agent
#		Important Note: It is the responsability of the Script User to have the right build version (SPI or UART) of executables
#
#		Recommended way is to use Symbolic links:
#		.e.g with IW612 Release Zip files to use SPI mode:
#		imx-linux-othost-xxx-otbr-yyy-rcp-zzz-date-revision.zip
#		NXP-ZBOSS-HOST-RELEASE-XXX.YY-xxxxxx-yyyyyy.zip
#		3 Zip files are unzipped in /root folder
#		ln -sf /root/imx-linux-othost-xxx-otbr-yyy-rcp-zzz-date-revision/openthread/usr/sbin/ot-ctl 		/usr/sbin/ot-ctl
#		ln -sf /root/imx-linux-othost-xxx-otbr-yyy-rcp-zzz-date-revision/openthread/usr/sbin/otbr-agent 	/usr/sbin/otbr-agent
#		ln -sf /root/imx-linux-othost-xxx-otbr-yyy-rcp-zzz-date-revision/openthread/usr/bin/ot-ctl 	/usr/bin/ot-ctl
#		ln -sf /root/imx-linux-othost-xxx-otbr-yyy-rcp-zzz-date-revision/openthread/usr/bin/ot-daemon 	/usr/bin/ot-daemon
#		ln -sf /root/NXP-ZBOSS-HOST-RELEASE-XXX.YY-xxxxxx-yyyyy/bin/zb_mux 				/usr/sbin/zb_mux
#
#	-> It doesn't use anymore nor modify the /etc/default/otbr-agent file
#		Openthread Arguments are directly built in OT_ARGS variable
#
#	-> It will enable OTBR Firewall through the function otbr_firewall_config() called when otbr-agent is started.
#################################################################################################################################################
#
#################################################################################################################################################
# Usage:
#	source start_openthread.sh {start|stop|redirect} -ot="Openthread Host Application"
#
#	-> Parameters:
#	Mandatory:
#	"-ot=string"		Openthread Host Application				string is "ot-daemon" or "otbr-agent"
#
#	Optional:
#	"-d=x":				Openthread Log Level					Range is [1:5] for ot-daemon, [1:7] for otbr-agent	default is "-d=1"
#	"-b=string":		Infrastructure/Backbone Interface		string is ethX or mlanX								default is "-B=eth0"
#	"-s=number":		SPI bus frequency						Range is [1000000:10000000]							default is "-s=1000000"
#	"-rcp=string":		RCP Mode								string is SPI or DUAL or UART						default is "-rcp=SPI"
#	"-region=string":	enable OT TX power table feature		Range is [0:1]										no default
#	"-rw":				For Reworked i.MX8M Mini				NA													NA
#	"-cpu=string":		Running CPU (future use):				string is CPU1 or CPU2								default is "-cpu=CPU1"
#################################################################################################################################################
#
#################################################################################################################################################
# Examples:
# source ./start_openthread.sh start -ot=ot-daemon
# source ./start_openthread.sh start -ot=otbr-agent
# source ./start_openthread.sh start -ot=otbr-agent -region=US -d=6 -rcp=SPI -B=mlan0 -s=7000000
# source ./start_openthread.sh start -ot=ot-daemon -region=US -d=5 -rcp=SPI -s=2000000 -rw
# source ./start_openthread.sh start -ot=otbr-agent -region=EU -d=6 -rcp=DUAL
# source ./start_openthread.sh start -ot=otbr-agent -region=EU -d=6 -rcp=SPI
#################################################################################################################################################


NO_COLOR='\033[0;10m'
NO_COLOR_BOLD='\033[1;10m'
NO_COLOR_HIGHLIGHT='\033[7;48m'
COLOR_GREEN='\033[1;32m'
COLOR_RED='\033[1;31m'
COLOR_RED_HIGHLIGHT='\033[7;31m'
COLOR_YELLOW='\033[1;33m'
COLOR_YELLOW_HIGHLIGHT='\033[7;33m'
function usage(){
	echo -e "${COLOR_GREEN}
# Usage:
#	source start_openthread.sh {start|stop|redirect} -ot=\"Openthread Host Application\"
#
#	-> Parameters:
#	Mandatory:
#	\"-ot=string\"\t\tOpenthread Host Application\t\tstring is \"ot-daemon\" or \"otbr-agent\"
#
#	Optional:
#	\"-d=x\":\t\t\tOpenthread Log Level\t\t\tRange is [1:5] for ot-daemon, [1:7] for otbr-agent\tdefault is \"-d=1\"
#	\"-b=string\":\t\tInfrastructure/Backbone Interface\tstring is ethX or mlanX\t\t\t\t\tdefault is \"-B=eth0\"
#	\"-s=number\":\t\tSPI bus frequency\t\t\tRange is [1000000:10000000]\t\t\t\tdefault is \"-s=1000000\"
#	\"-rcp=string\":\t\tRCP Mode\t\t\t\tstring is SPI or DUAL or UART\t\t\t\tdefault is \"-rcp=SPI\"
#	\"-region=string\":\tenable OT TX power table feature\tRange is [0:1]\t\t\t\t\t\tno default
#	\"-rw\":\t\t\tFor Reworked i.MX8M Mini\t\tNA\t\t\t\t\t\t\tNA
#	\"-cpu=string\":\t\tRunning CPU (future use):\t\tstring is CPU1 or CPU2\t\t\t\t\tdefault is \"-cpu=CPU1\"
#
# Examples:
# source ./start_openthread.sh start -ot=ot-daemon
# source ./start_openthread.sh start -ot=otbr-agent
# source ./start_openthread.sh start -ot=otbr-agent -region=US -d=6 -rcp=SPI -B=mlan0 -s=7000000
# source ./start_openthread.sh start -ot=ot-daemon -region=US -d=5 -rcp=SPI -s=2000000 -rw
# source ./start_openthread.sh start -ot=otbr-agent -region=EU -d=6 -rcp=DUAL
# source ./start_openthread.sh start -ot=otbr-agent -region=EU -d=6 -rcp=SPI
${NO_COLOR}"
}

#OT_DIR will be created if it does not exist
OT_DIR=~/openthread

# Openthread application to use, mandatory parameter
OT_HOST_APPLICATION=""

#OT_LOG will be created if it does not exist
OT_LOG=${OT_DIR}/logs
LOGFILE=""

DEFAULT_OT_LOG_LEVEL=1
DEFAULT_SPI_SPEED=1000000

DEFAULT_OTBR_FILE=/etc/default/otbr-agent
SYSCTL_IP_FORWARD=/etc/sysctl.d/60-otbr-ip-forward.conf
SYSCTL_ACCEPT_RA_FILE=/etc/sysctl.d/60-otbr-accept-ra.conf

# RCP Bus to use: SPI or UART or DualPan
OT_RCP_MODE=SPI

# CPU to use: CPU1 or CPU2 or UART
DEFAULT_CPU=CPU1


function detect_imx_platform()
{
	# Check hardware setting to adapt config
	echo ""
	soc_id=`cat /sys/devices/soc0/soc_id`
}

function get_gpiochip_spidev_cfg() {
	echo "From device-tree settings -> Retrieve spidev numbers, IW61x_OT_RST and IW61w_OT_INT gpiochip lines"

	RST_gpiochip_num=""
	RST_gpiochip_line=""
	INT_gpiochip_num=""
	INT_gpiochip_line=""
	SPI_dev=""

	RST_gpiochip_num=`gpioinfo IWxxx_NB_IND_RST_15_4 2>/dev/null | awk -F" " '{print $1}'`
	RST_gpiochip_line=`gpioinfo IWxxx_NB_IND_RST_15_4 2>/dev/null | awk -F" " '{print $2}'`
	INT_gpiochip_num=`gpioinfo IWxxx_NB_SPI_INT 2>/dev/null | awk -F" " '{print $1}'`
	INT_gpiochip_line=`gpioinfo IWxxx_NB_SPI_INT 2>/dev/null | awk -F" " '{print $2}'`

	spi0=`find /sys/firmware/devicetree/base -name spi@0`
	if [ -f $spi0/label ] && [ `tr -d '\0' < $spi0/label` = "IWxxx_SPIDEV" ]; then
		num=$(echo $spi0 | awk -F/ '{print NF}')
		let num=num-1
		spi_bus_addr=$(echo $spi0 | cut -d '/' -f$num | cut -d "@" -f2)
		spi_folder=$(find /sys/devices/platform -type d -name $spi_bus_addr.spi*)
		SPI_dev=$(find $spi_folder -type d -name spidev*.* | awk -F/ '{print $NF}')
	fi

	if [ ${REWORK} -ne 0 ]; then
		if [ ${soc_id} == "i.MX8MM" ]; then
			INT_gpiochip_num=gpiochip4
			INT_gpiochip_line=5
			echo "!!! Reworked iM8MMini !!!"
		else
			echo -e "${COLOR_RED}------> Error: Reworked parameter is ONLY for i.MX8M mini variant ${NO_COLOR}"
			return -1
		fi
	fi

	echo -e "${COLOR_GREEN}SPI device:   $SPI_dev${NO_COLOR}"
	echo -e "${COLOR_GREEN}RST:          $RST_gpiochip_num:$RST_gpiochip_line${NO_COLOR}"
	echo -e "${COLOR_GREEN}INT:          $INT_gpiochip_num:$INT_gpiochip_line${NO_COLOR}"
	if [ -z ${RST_gpiochip_num} ] || [ -z ${RST_gpiochip_line} ] || [ -z ${INT_gpiochip_num} ] || [ -z ${INT_gpiochip_line} ] || [ -z ${SPI_dev} ]; then
		echo -e "${COLOR_RED}------> Error: Impossible to retrieve gpiochip number/line or spidev number from Device Tree, dtb file doesn't support gpiochip Naming !!!${NO_COLOR}"
		return -1
	fi

	export SPI_dev=$SPI_dev
	export RST_gpiochip_num=$RST_gpiochip_num
	export RST_gpiochip_line=$RST_gpiochip_line
	export INT_gpiochip_num=$INT_gpiochip_num
	export INT_gpiochip_line=$INT_gpiochip_line
}

function stop_log_redirection(){
	if [ -f ${OT_LOG}/.parentlog.pid ]; then
		parentlog_pid=$(cat ${OT_LOG}/.parentlog.pid)
		if [ ! -z $parentlog_pid ]; then 
			#pkill -P kills the parent background process which creates logs and its children (journalctl and grep)
			pkill -P $parentlog_pid
		fi
		echo "" > ${OT_LOG}/.parentlog.pid
	fi
}

function start_log_redirection(){
	if [ ! -d ${OT_DIR} ]; then
		mkdir ${OT_DIR}
	fi
	if [ ! -d ${OT_LOG} ]; then
		mkdir ${OT_LOG}
	fi

	stop_log_redirection
	if [ -z ${LOGFILE} ]; then 
		LOGFILE=${OT_HOST_APPLICATION}"_`date +"%y%m%d-%H%M%S"`.log"
	fi
	LOGFILE=${OT_LOG}/${LOGFILE}
	(journalctl -f | grep ${OT_HOST_APPLICATION} --line-buffered) >> ${LOGFILE} 2>&1 &
	RETVAL=$?
	PID=$!
	# $PID will hold the PID of the parent background process that keeps producing logs
	[ $RETVAL -eq 0 ] && echo $PID > ${OT_LOG}/.parentlog.pid 
	echo "${OT_HOST_APPLICATION} log: ${LOGFILE}"
}

function start_zb_mux() {
	echo -e "${COLOR_GREEN}DUAL mode selected, start zb_mux${NO_COLOR}"
	nohup /usr/sbin/zb_mux -i "/dev/$SPI_dev" -o 0:/tmp/ttyOpenThread -o 2:/tmp/ttyZigbee -s -S $1 -m 0 -I $INT_gpiochip_line:/dev/$INT_gpiochip_num -t 0:0 1>/dev/null 2>&1 &
}

function set_spifreq() {
	case ${OT_RCP_MODE} in
		"SPI")
			OT_ARGS="-d $OT_LOG_LEVEL spinel+spi:///dev/$SPI_dev?gpio-reset-device=/dev/$RST_gpiochip_num&gpio-int-device=/dev/$INT_gpiochip_num&gpio-int-line=$INT_gpiochip_line&gpio-reset-line=$RST_gpiochip_line&spi-mode=0&spi-speed=$DEFAULT_SPI_SPEED"
		;;
		"DUAL")
			start_zb_mux $DEFAULT_SPI_SPEED
			OT_ARGS="-d $OT_LOG_LEVEL spinel+hdlc+uart:///tmp/ttyOpenThread "
		;;
		"UART")
			OT_ARGS="-d $OT_LOG_LEVEL spinel+hdlc+uart:///dev/ttyACM0 "
		;;
	esac
	if [ ${OT_HOST_APPLICATION} == "otbr-agent" ]; then
		OT_ARGS="${OT_ARGS} -I wpan0 -B ${INFRA_IF_NAME} trel://${INFRA_IF_NAME} "
	fi

	nohup ${PATH_TO_OT_HOST_APPLICATION}/${OT_HOST_APPLICATION} ${OT_ARGS} 1>/dev/null 2>&1 &
	sleep 2
	echo -e "${COLOR_GREEN}-> Factory Reset${NO_COLOR}"
	/usr/bin/ot-ctl factoryreset
	sleep 2
	echo -e "${COLOR_GREEN}-> Set spifreq at $SPI_SPEED${NO_COLOR}"
	/usr/bin/ot-ctl spifreq $SPI_SPEED
	echo -e "${COLOR_GREEN}-> Openthread Version${NO_COLOR}"
	/usr/bin/ot-ctl version
	echo -e "${COLOR_GREEN}-> Firmware Version${NO_COLOR}"
	/usr/bin/ot-ctl fwversion
	echo -e "${COLOR_GREEN}-> Get spifreq${NO_COLOR}"
	/usr/bin/ot-ctl spifreq
	sleep 1
	stop_openthread
}

function start_openthread() {
	if [ -z ${OT_LOG_LEVEL} ]; then
		OT_LOG_LEVEL=${DEFAULT_OT_LOG_LEVEL}
	fi
	if [ -z ${INFRA_IF_NAME} ]; then 
		INFRA_IF_NAME="${INFRA_IF_NAME:-eth0}"
	fi
	if [ -z ${SPI_SPEED} ]; then
		SPI_SPEED=${DEFAULT_SPI_SPEED}
	fi
	if [ -z ${CPU} ]; then
		CPU=${DEFAULT_CPU}
	fi

	set_spifreq
	sleep 2

	case ${OT_RCP_MODE} in
		"SPI")
			OT_ARGS="-d $OT_LOG_LEVEL spinel+spi:///dev/$SPI_dev?gpio-reset-device=/dev/$RST_gpiochip_num&gpio-int-device=/dev/$INT_gpiochip_num&gpio-int-line=$INT_gpiochip_line&gpio-reset-line=$RST_gpiochip_line&spi-mode=0&spi-speed=$SPI_SPEED&spi-reset-delay=500"
		;;
		"DUAL")
			start_zb_mux $SPI_SPEED
			OT_ARGS="-d $OT_LOG_LEVEL spinel+hdlc+uart:///tmp/ttyOpenThread"
		;;
		"UART")
			OT_ARGS="-d $OT_LOG_LEVEL spinel+hdlc+uart:///dev/ttyACM0"
		;;
	esac
	# WARNING : NOTE that OT_ARGS defined above MUST NOT HAVE A SPACE AS LAST CHARACTER, THIS IS TO ADD "&region=" IF NEEDED
	if [ ! -z "${REGION_NAME}" ]; then
		if [ ${OT_RCP_MODE} == "UART" ] || [ ${OT_RCP_MODE} == "DUAL" ]; then
			OT_ARGS="${OT_ARGS}?region=${REGION_NAME}"
		fi
		if [ ${OT_RCP_MODE} == "SPI" ]; then
			OT_ARGS="${OT_ARGS}&region=${REGION_NAME}"
		fi
	fi

	if [ ${OT_HOST_APPLICATION} == "otbr-agent" ]; then
		OT_ARGS="${OT_ARGS} -I wpan0 -B ${INFRA_IF_NAME} trel://${INFRA_IF_NAME} "
		echo -e "${COLOR_GREEN}${OT_HOST_APPLICATION} Dependencies Start${NO_COLOR}"
		ln -sf /usr/sbin/xtables-legacy-multi /usr/sbin/ip6tables
		sleep 1
		rt_tables_install
		accept_ra_enable
		ipforward_install_and_config
		otbr_firewall_config
		systemctl start mdns
		dhcpv6_pd_ref_install
		wired_network_update
		systemctl restart systemd-networkd
	fi

	start_log_redirection
	if [ -f ${OT_DIR}/debug ]; then
		echo " Configure core file generation in ${OT_DIR}/core"
		mkdir -p ${OT_DIR}/core
		# Core file name:
		# - %e: executable file name
		# - %p: pid
		# - %t: time
		# - %s: number of the signal causing dump
		echo "${OT_DIR}/core/%e-%p-%t.signal-%s.core" > /proc/sys/kernel/core_pattern
		ulimit -c unlimited
	fi

	echo ""
	echo -e "${COLOR_GREEN}Starting : ${PATH_TO_OT_HOST_APPLICATION}/${OT_HOST_APPLICATION}"
	echo "Args     : $OT_ARGS"
	echo "Log file : ${LOGFILE}"
	echo -e "${NO_COLOR}"
	sleep 0.5
	#Launching OT_HOST_APPLICATION SHOULD BE the last command of this Shell script 
	nohup ${PATH_TO_OT_HOST_APPLICATION}/${OT_HOST_APPLICATION} ${OT_ARGS} 1>/dev/null 2>&1 &
}

function stop_openthread() {
	stop_log_redirection
	killall -q ot-daemon
	killall -q otbr-agent
	killall -q zb_mux
	systemctl stop mdns
}

function rt_tables_install()
{
	#Finding the Routing Table Location Dynamically (It often changes with BSP versions)
	RT_TABLES_PATH=$(find / -name rt_tables)

	#Adding Routing Table mapping for Openthread 
	if grep -q "88 openthread" ${RT_TABLES_PATH}; then
		echo -e "The routing table ${RT_TABLES_PATH} is already configured (it already includes routing table mapping for openthread ==> 88 openthread) \n"
	else
		echo -e "Appending \"88 openthread\" to the file ${RT_TABLES_PATH}\n"
		echo "88 openthread" >> ${RT_TABLES_PATH}
	fi

	if [[ $(cat /proc/sys/net/core/optmem_max) -eq 65536 ]]; then
		echo -e "the file /etc/sysctl.conf is already configured, the value of net.core.optmem_max=65536.\n"
	else
		sudo sh -c 'echo "" >>/etc/sysctl.conf'
		sudo sh -c 'echo "# OpenThread configuration" >>/etc/sysctl.conf'
		sudo sh -c 'echo "net.core.optmem_max=65536" >>/etc/sysctl.conf'
		sudo sh -c 'sysctl -p /etc/sysctl.conf'
	fi

}

function accept_ra_enable()
{
if [ ! -f $SYSCTL_ACCEPT_RA_FILE ]; then
echo -e "File $SYSCTL_ACCEPT_RA_FILE was created to enable Router Advertisements, contents below: \n"
	sudo tee $SYSCTL_ACCEPT_RA_FILE <<EOF
net.ipv6.conf.${INFRA_IF_NAME}.accept_ra = 2
net.ipv6.conf.${INFRA_IF_NAME}.accept_ra_rt_info_max_plen = 64
EOF
echo -e "\n"

else
	echo -e "Configuration file $SYSCTL_ACCEPT_RA_FILE exists already\n"
fi
	if [ -f /proc/sys/net/ipv6/conf/"${INFRA_IF_NAME}"/accept_ra ]; then
		echo 2 | sudo tee /proc/sys/net/ipv6/conf/"${INFRA_IF_NAME}"/accept_ra > /dev/null || die 'Failed to enable IPv6 RA!'
	fi

	if [ -f /proc/sys/net/ipv6/conf/"${INFRA_IF_NAME}"/accept_ra_rt_info_max_plen ]; then
		echo 64 | sudo tee /proc/sys/net/ipv6/conf/"${INFRA_IF_NAME}"/accept_ra_rt_info_max_plen > /dev/null || die 'Failed to enable IPv6 RIO!'
	fi
}

function ipforward_install_and_config()
{
if [ ! -f $SYSCTL_IP_FORWARD ]; then
echo -e "File $SYSCTL_IP_FORWARD was created to enable IP forwarding, contents below: \n"

	sudo tee $SYSCTL_IP_FORWARD <<EOF
net.ipv6.conf.all.forwarding = 1
net.ipv4.ip_forward = 1
EOF
echo -e "\n"
else
	echo -e "Configuration file $SYSCTL_IP_FORWARD exists already\n"
fi

	echo 1 | sudo tee /proc/sys/net/ipv6/conf/all/forwarding > /dev/null || die 'Failed to enable IPv6 forwarding!'
	echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward > /dev/null || die 'Failed to enable IPv4 forwarding!'
}


function otbr_firewall_config()
{
	ipset create -exist otbr-ingress-deny-src hash:net family inet6
	sleep 1
	ipset create -exist otbr-ingress-deny-src-swap hash:net family inet6
	sleep 1
	ipset create -exist otbr-ingress-allow-dst hash:net family inet6
	sleep 1
	ipset create -exist otbr-ingress-allow-dst-swap hash:net family inet6
	sleep 1
	ip6tables -N OTBR_FORWARD_INGRESS
	sleep 1
	ip6tables -I FORWARD 1 -o wpan0 -j OTBR_FORWARD_INGRESS
	sleep 1
	ip6tables -A OTBR_FORWARD_INGRESS -m pkttype --pkt-type unicast -i wpan0 -j DROP
	sleep 1
	ip6tables -A OTBR_FORWARD_INGRESS -m set --match-set otbr-ingress-deny-src src -j DROP
	sleep 1
	ip6tables -A OTBR_FORWARD_INGRESS -m set --match-set otbr-ingress-allow-dst dst -j ACCEPT
	sleep 1
	ip6tables -A OTBR_FORWARD_INGRESS -m pkttype --pkt-type unicast -j DROP
	sleep 1
	ip6tables -A OTBR_FORWARD_INGRESS -j ACCEPT
	sleep 3

	#below commands are to active NAT64 and NAT44 
	iptables -t mangle -A PREROUTING -i wpan0 -j MARK --set-mark 0x1001
	iptables -t nat -A POSTROUTING -m mark --mark 0x1001 -j MASQUERADE
	iptables -t filter -A FORWARD -o ${INFRA_IF_NAME} -j ACCEPT
	iptables -t filter -A FORWARD -i ${INFRA_IF_NAME} -j ACCEPT
}

UPSTREAM_INTERFACE="eth0"
WPAN_INTERFACE="wpan0"

PD_DAEMON_DIR="/etc/dhcpv6"
DHCPCD_ENTER_HOOK="${PD_DAEMON_DIR}/dhcpcd.enter-hook"
DHCPCD_EXIT_HOOK="${PD_DAEMON_DIR}/dhcpcd.exit-hook"

PD_DAEMON_PATH="${PD_DAEMON_DIR}/dhcp6_pd_daemon.py"
PD_DAEMON_SERVICE_NAME="dhcp6_pd_daemon.service"
PD_DAEMON_SERVICE_PATH="/etc/systemd/system/${PD_DAEMON_SERVICE_NAME}"

DHCP_CONFIG_PATH="/etc/dhcpcd.conf"
DHCP_CONFIG_ORIG_PATH="/etc/dhcpcd.conf.orig"
DHCP_CONFIG_PD_PATH="/etc/dhcpcd.conf.pd"
DHCP_CONFIG_NO_PD_PATH="/etc/dhcpcd.conf.no-pd"

# Create dhcpcd configuration file with ipv6 prefix request.
function create_dhcpcd_conf_pd()
{
    sudo tee ${DHCP_CONFIG_PD_PATH} >/dev/null <<EOF
noipv6rs # disable router solicitation
interface ${UPSTREAM_INTERFACE}
  iaid 1
#  ia_pd 2/::/64 -
   ia_pd 2/::/64 ${WPAN_INTERFACE}/0/64
release
# Disable Router Solicitations (RS) again, specifically for ${UPSTREAM_INTERFACE}.
# This ensures that accept_ra is prevented from being set to 0, allowing
# the interface to accepting Router Advertisements and configuring IPv6
# based on them. The exact reason for requiring 'noipv6rs' twice
# is not fully understood but has been observed to be necessary through
# experimentation.
noipv6rs
EOF
}

# Create dhcpcd configuration file with no prefix request.
function create_dhcpcd_conf_no_pd()
{
    sudo tee ${DHCP_CONFIG_NO_PD_PATH} >/dev/null <<EOF
noipv6rs # disable router solicitation
EOF
}

function create_dhcp6_pd_daemon_service()
{
    sudo tee ${PD_DAEMON_SERVICE_PATH} <<EOF
[Unit]
Description=Daemon to manage dhcpcd based on otbr-agent's PD state change
ConditionPathExists=${PD_DAEMON_PATH}
Requires=otbr-agent.service
After=otbr-agent.service

[Service]
Type=simple
User=root
ExecStart=/usr/bin/python3 ${PD_DAEMON_PATH}
Restart=on-failure

[Install]
WantedBy=multi-user.target
EOF
}

function dhcpv6_pd_ref_uninstall()
{
    sudo systemctl disable ${PD_DAEMON_SERVICE_NAME} || true
    sudo systemctl stop ${PD_DAEMON_SERVICE_NAME} || true
    sudo rm -f ${PD_DAEMON_SERVICE_PATH} || true

    if [[ -f ${DHCP_CONFIG_ORIG_PATH} ]]; then
        sudo mv ${DHCP_CONFIG_ORIG_PATH} ${DHCP_CONFIG_PATH}
    fi

    sudo rm -f ${DHCPCD_ENTER_HOOK} ${DHCPCD_EXIT_HOOK}
    sudo rm -f ${PD_DAEMON_PATH}

    sudo systemctl daemon-reload

    if systemctl is-active dhcpcd; then
    	sudo systemctl restart dhcpcd || true
    fi
}

function dhcpv6_pd_ref_install()
{
    if [[ -f ${DHCP_CONFIG_PATH} ]]; then
        sudo mv ${DHCP_CONFIG_PATH} ${DHCP_CONFIG_ORIG_PATH}
    fi

    create_dhcpcd_conf_pd
    create_dhcpcd_conf_no_pd
    create_dhcp6_pd_daemon_service

    # The dhcp6_pd_daemon is currently disabled because it restarts dhcpcd
    # when the PD state changes. This restart disrupts mDNS, causing
    # connectivity issues. The daemon and its associated systemd service
    # files are still installed for potential future use.
    #
    # TODO: Re-enable and start the daemon when a solution is found
    #       for dhcpcd restarts breaking mDNS.
    #
    # if have systemctl; then
    #    sudo systemctl daemon-reload
    #    sudo systemctl enable ${PD_DAEMON_SERVICE_NAME}
    #    sudo systemctl start ${PD_DAEMON_SERVICE_NAME}
    # fi

    # Always enable PD, which is a workaround for the currently disabled
    # dhcp6_pd_daemon which caused mDNS disruptions.
    sudo cp ${DHCP_CONFIG_PD_PATH} ${DHCP_CONFIG_PATH}

    sudo systemctl daemon-reload

    # Restart dhcpcd only if it's running. This is unnecessary when the dhcp6_pd_daemon
    # is enabled, as the daemon will handle dhcpcd restarts based on PD state changes.
    if systemctl is-active dhcpcd; then
    	sudo systemctl restart dhcpcd || true
    fi


    echo "DCHPv6 PD daemon installed"
}

NETWORK_CONFIG_PATH="/usr/lib/systemd/network/80-wired.network"
function wired_network_update()
{
    sudo tee ${NETWORK_CONFIG_PATH} <<EOF
[Match]
Name=eth0

[Network]
DHCP=yes
IPv6AcceptRA=yes
DHCPPrefixDelegation=yes

[DHCPv6]
UseDelegatedPrefix=yes
WithoutRA=solicit
DUIDType=link-layer
IAID=0

[DHCPPrefixDelegation]
UplinkInterface=:self
SubnetId=0
Assign=yes
Token=::1~
EOF
}

function main()
{
	REWORK=0
	for i in "$@"; do
	  case $i in
		-l=*)
			LOGFILE="${i#*=}"
			shift 
		;;
		-ot=*)
			OT_HOST_APPLICATION="${i#*=}"
			case ${OT_HOST_APPLICATION} in
				ot-daemon)
					PATH_TO_OT_HOST_APPLICATION=/usr/bin
				;;
				otbr-agent)
					PATH_TO_OT_HOST_APPLICATION=/usr/sbin
				;;
				*)
					echo -e "${COLOR_RED}------> Error: Missing \"-ot\" mandatory parameter: -ot=ot-daemon or -ot=otbr-agent${NO_COLOR}"
					usage
					return -1
			esac
			shift
		;;
		-d=*)
			OT_LOG_LEVEL="${i#*=}"
			MAX_LOG_LEVEL=5
			if [ ${OT_HOST_APPLICATION} == "otbr-agent" ]; then
				MAX_LOG_LEVEL=7
			fi
			if [ ${OT_LOG_LEVEL} -lt 1 ] || [ ${OT_LOG_LEVEL} -gt ${MAX_LOG_LEVEL} ];then
				echo -e "${COLOR_RED}------> Error: \"-d\" parameter is not set correctly: OT_LOG_LEVEL must be in [1:${MAX_LOG_LEVEL}] for ${OT_HOST_APPLICATION}${NO_COLOR}"
				usage
				return -1
			fi
			shift
		;;
		-b=*)
			INFRA_IF_NAME="${i#*=}"
			if ! [[ ${INFRA_IF_NAME} =~ "eth" ]] && ! [[ ${INFRA_IF_NAME} =~ "mlan" ]]; then
				echo -e "${COLOR_RED}------> Error: \"-b\" parameter is not set correctly: INFRA_IF_NAME must be either ethx or mlanx${NO_COLOR}"
				usage
				return -1
			fi
			shift
		;;
		-s=*)
			SPI_SPEED="${i#*=}"
			if [ ${SPI_SPEED} -lt 1000000 ] || [ ${SPI_SPEED} -gt 10000000 ];then
				echo -e "${COLOR_RED}------> Error: \"-s\" parameter is not set correctly: SPI_SPEED must be in [1000000:10000000]${NO_COLOR}"
				usage
				return -1
			fi
			shift
		;;
		-rcp=*)
			OT_RCP_MODE="${i#*=}"
			if [ ! ${OT_RCP_MODE} == "SPI" ] && [ ! ${OT_RCP_MODE} == "UART" ] && [ ! ${OT_RCP_MODE} == "DUAL" ];then
				echo -e "${COLOR_RED}------> Error: \"-rcp\" parameter is not set correctly: SPI or UART or DUAL must be used${NO_COLOR}"
				usage
				return -1
			fi
			shift
		;;
		-region=*)
			REGION_NAME="${i#*=}"
			if [[ ! ${REGION_NAME} =~ ^[A-Z] ]]; then
				echo -e "${COLOR_RED}------> Error: region must be defined in CAPITAL letter. i.e, -region=${REGION_NAME^^}${NO_COLOR}"
				usage
				return -1
			fi
			if [ ! -f /data/openthread/openthread.conf ]; then
				echo -e "${COLOR_RED}------> Error: /data/openthread/openthread.conf is missing, impossible to set region parameter${NO_COLOR}"
				usage
				return -1
			fi
			shift
		;;
		-rw)
			REWORK=1
		;;
		-cpu=*)
			CPU="${i#*=}"
			shift 
		;;
		-*|--*)
			usage
			return -1
		;;
		start)
			ACTION="start"
			shift
		;;
		stop)
			ACTION="stop"
			shift
		;;
		redirect)
			ACTION="redirect"
			shift
		;;
		*)
		;;
	  esac
	done

	case ${ACTION} in
		start)
			stop_openthread
			detect_imx_platform
			get_gpiochip_spidev_cfg
			if [ $? -ne 0 ]; then
				usage
				return -1
			fi
			start_openthread
		;;
		stop)
			stop_openthread
		;;
		redirect)
			start_log_redirection
		;;
		*)
			usage
			return -1
	 esac
	return 0
}

main "$@"
