"""
Connect all midi devices to each other!
"""

import sys
import logging
import subprocess
import re

__author__ = 'Stephen Brown (Little Fish Solutions LTD)'


if sys.version_info.major < 3:
    sys.exit('Sorry, this script only supports Python 3')

log = logging.getLogger(__name__)

VERSION = '0.0.0'

VERBOSE = False

if __name__ == '__main__':
    if VERBOSE:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO, format='%(message)s')

    log.info('Midi Connect v{}'.format(VERSION))
    
    # Run aconnect and get the output
    aconnect_process = subprocess.run(['aconnect', '-i', '-l'], stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
    
    # Keep a list of clients
    clients = {}

    # Keep a list of existing mappings (from, to)
    existing_connections = []

    def add_client_port(client_number, port_number):
        if client_number not in clients:
            clients[client_number] = []

        clients[client_number].append(port_number)
    
    # Regular expressions used for parsing
    client_regex = re.compile(r'^client ([0-9]+): \'([^\']*)\'.*$')
    port_regex = re.compile(r'^\s*([0-9]+) \'([^\']*)\'.*$')
    connecting_to_regex = re.compile(r'^\s*Connecting To: (.*)$')
    connecting_from_regex = re.compile(r'^\sConnected From:.*$')

    client_number = None
    client_name = None
    skip_client = False

    # break up into lines for processing
    lines = aconnect_process.stdout.decode().split('\n')
    for line in lines:
        if not line.strip():
            continue

        client_match = client_regex.match(line)
        if client_match:
            client_number = client_match.group(1)
            client_name = client_match.group(2)

            log.debug('Found client {} "{}"'.format(client_number, client_name))

            skip_client = client_name in ['System', 'Midi Through']
            if skip_client:
                log.debug(' - Skipping client "{}"'.format(client_name))
            
            continue
       
        port_match = port_regex.match(line)
        if port_match:
            if client_number is None:
                raise Exception('Found port without client: {}'.format(line))

            if skip_client:
                continue

            port_number = port_match.group(1)
            port_name = port_match.group(2)

            log.debug(' - Found port {} "{}"'.format(port_number, port_name))
            add_client_port(client_number, port_number)

            continue

        connecting_to_match = connecting_to_regex.match(line)
        if connecting_to_match:
            connecting_from = '{}:{}'.format(client_number, port_number)
            connecting_to_str = connecting_to_match.group(1)
            connecting_to_list = [s.strip() for s in connecting_to_str.split(',')]
            for connecting_to in connecting_to_list:
                log.debug('Existing connection {} -> {}'.format(connecting_from, connecting_to))
                existing_connections.append((connecting_from, connecting_to))

            continue

        connecting_from_match = connecting_from_regex.match(line)
        if connecting_from_match:
            continue
        
        raise Exception('Unhandled line: {}'.format(line))
    
    # We now have our clients dict
    for (from_client_number, from_ports) in clients.items():
        for (to_client_number, to_ports) in clients.items():
            if from_client_number == to_client_number:
                # Don't map things onto themselves
                continue

            for from_port in from_ports:
                for to_port in to_ports:
                    from_str = '{}:{}'.format(from_client_number, from_port)
                    to_str = '{}:{}'.format(to_client_number, to_port)
                    
                    if (from_str, to_str) in existing_connections:
                        log.info('{} is already connected to {} - skipping'.format(
                            from_str, to_str
                        ))

                        continue

                    log.info('Mapping {} -> {}'.format(from_str, to_str))

                    map_process = subprocess.run([
                        'aconnect', from_str, to_str
                    ], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

                    output = map_process.stdout
                    if output:
                        log.warning(output.decode())
        