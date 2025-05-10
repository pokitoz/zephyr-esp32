# See https://github.com/zephyrproject-rtos/docker-image
FROM ghcr.io/zephyrproject-rtos/zephyr-build:v0.26-branch

ARG USERNAME=pokitoz
ARG USER_UID=1000
ARG USER_GID=$USER_UID

USER $USERNAME

# Setup zephyr environment at login, save history to file.
RUN echo "source ${NCS_BASE}/ncs/zephyr/zephyr-env.sh" >> /home/${USERNAME}/.bashrc \
    && printf "prompt() {\n history -a\n}\nexport PROMPT_COMMAND=prompt" >> /home/${USERNAME}/.bashrc
