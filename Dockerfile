FROM  ubuntu:20.04 as builder

USER  root

ENV OPT /opt/wtsi-cgp
ENV PATH $OPT/bin:$PATH
ENV LD_LIBRARY_PATH $OPT/lib
ENV LC_ALL C

RUN apt-get -yq update
RUN apt-get install -yq --no-install-recommends build-essential
RUN apt-get install -yq --no-install-recommends apt-transport-https
RUN apt-get install -yq --no-install-recommends curl
RUN apt-get install -yq --no-install-recommends ca-certificates
RUN apt-get install -yq --no-install-recommends make
RUN apt-get install -yq --no-install-recommends zlib1g-dev
RUN apt-get install -yq --no-install-recommends libbz2-dev
RUN apt-get install -yq --no-install-recommends liblzma-dev
RUN apt-get install -yq --no-install-recommends libcurl4-gnutls-dev
RUN apt-get install -yq --no-install-recommends libssl-dev

COPY . .
RUN bash setup.sh $OPT

FROM  ubuntu:20.04

LABEL maintainer="cgphelp@sanger.ac.uk"\
      uk.ac.sanger.cgp="Cancer, Ageing and Somatic Mutation, Wellcome Sanger Institute" \
      version="0.5.0" \
      description="sequence_utils"

ENV OPT /opt/wtsi-cgp
ENV PATH $OPT/bin:$PATH
ENV LD_LIBRARY_PATH $OPT/lib
ENV LC_ALL C

RUN apt-get -yq update
RUN apt-get install -yq --no-install-recommends \
apt-transport-https \
curl \
ca-certificates \
bzip2 \
zlib1g \
liblzma5 \
libcurl3-gnutls \
unattended-upgrades && \
unattended-upgrade -d -v && \
apt-get remove -yq unattended-upgrades && \
apt-get autoremove -yq

RUN mkdir -p $OPT
COPY --from=builder $OPT $OPT

## USER CONFIGURATION
RUN adduser --disabled-password --gecos '' ubuntu && chsh -s /bin/bash && mkdir -p /home/ubuntu

USER    ubuntu
WORKDIR /home/ubuntu

CMD ["/bin/bash"]
