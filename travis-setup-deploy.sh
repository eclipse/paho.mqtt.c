openssl aes-256-cbc -K $encrypted_dcd2b299f7c9_key -iv $encrypted_dcd2b299f7c9_iv -in deploy_rsa.enc -out /tmp/deploy_rsa -d
eval "$(ssh-agent -s)"
chmod 600 /tmp/deploy_rsa
ssh-add /tmp/deploy_rsa
