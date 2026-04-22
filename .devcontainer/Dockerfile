
# use deeploy as base (todo: chimera container)
FROM ghcr.io/pulp-platform/deeploy:devel

ARG USERNAME=code
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# install clangd
RUN apt-get update && apt-get -y install clangd sudo

# Create the user
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    #
    # allow sudo
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

# copy root .zshrc and set permissions
RUN cp /root/.zshrc /home/$USERNAME/.zshrc && chown $USER_UID:$USER_GID /home/$USERNAME/.zshrc

# [Optional] Set the default user. Omit if you want to keep the default as root.
USER $USERNAME

# install oh-my-zsh also for the non-root user
RUN sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" "" --unattended

