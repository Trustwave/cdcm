set -e
cd build || exit
if [ "$RELEASE" = "RELEASE" ]; then
  curl -L "$NEXUS""service/local/artifact/maven/redirect?r=releases&g=com.trustwave&a=cdcm&v=RELEASE&p=rpm" -o cdcm.rpm
  if [ "$(rpm -qa | grep cdcm)" = "$(rpm -qp cdcm.rpm)" ]; then
    export INST_ACTION='reinstall'
  else
    export INST_ACTION='install'
  fi
  sudo yum $INST_ACTION -y cdcm.rpm
  sed -i 's/127.0.0.1:7613/0.0.0.0:7613/gm' /etc/cdcm/cdcm_settings.json
  systemctl restart cdcm
fi
