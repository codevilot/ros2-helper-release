"""`ros2 topic graphic` verb."""

import subprocess
import time

from ros2cli.plugin_system import satisfies_version
from ros2cli.verb import VerbExtension


DEMO_TOPICS = {
    'image': '/camera/image_raw',
    'compressed_image': '/camera/image_raw/compressed',
    'scan': '/scan',
    'joint_states': '/joint_states',
}


class GraphicVerb(VerbExtension):
    """Render a supported topic graphically in the terminal."""

    EXTENSION_POINT_VERSION = '0.1'

    def __init__(self):
        super().__init__()
        satisfies_version(VerbExtension.EXTENSION_POINT_VERSION, '^0.1')

    def add_arguments(self, parser, cli_name):
        parser.add_argument(
            'topic_name',
            nargs='?',
            help='Topic to render. Supported types: Image, CompressedImage, LaserScan, JointState.',
        )
        parser.add_argument(
            '--demo',
            choices=sorted(DEMO_TOPICS),
            help='Start a built-in demo publisher and render its topic.',
        )
        parser.add_argument(
            '--fps',
            type=float,
            default=None,
            help='Maximum rendering rate passed to ros2-helper-graphic.',
        )

    def main(self, *, args):
        if args.demo is None and args.topic_name is None:
            print('topic_name is required unless --demo is used')
            return 2

        topic_name = args.topic_name or DEMO_TOPICS[args.demo]
        command = [
            'ros2',
            'run',
            'ros2_helper',
            'ros2-helper-graphic',
            topic_name,
        ]
        if args.fps is not None:
            command.extend(['--fps', str(args.fps)])

        if args.demo is None:
            return subprocess.call(command)

        demo_command = [
            'ros2',
            'run',
            'ros2_helper',
            'ros2-helper-graphic-mockup',
            args.demo,
            '--topic',
            topic_name,
        ]
        demo_process = subprocess.Popen(
            demo_command,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )
        try:
            time.sleep(2)
            return subprocess.call(command)
        finally:
            demo_process.terminate()
            try:
                demo_process.wait(timeout=3)
            except subprocess.TimeoutExpired:
                demo_process.kill()
                demo_process.wait()
