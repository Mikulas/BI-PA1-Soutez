<?php

$tests = $argv;
unset($tests[0]);

if ($tests) {
	echo "running " . implode(', ', $tests) . " only\n\n";
}

$ok = TRUE;
$stats = (object) ['passed' => 0, 'failed' => 0];

$total_time = 0;
foreach (scandir('tests') as $dir)
{
	if (in_array($dir, ['.', '..'])) continue;

	echo "\033[1;45;30m  $dir  \033[0m\n\n";
	foreach (glob("tests/$dir/*.in") as $in)
	{
		if ($tests) {
			$matched = FALSE;
			foreach ($tests as $mask)
			{
				if (strpos($in, $mask) !== FALSE) {
					$matched = TRUE;
					break;
				}
			}
			if (!$matched) {
				continue;
			}
		}

		$out = str_replace('.in', '.out', $in);
		$cin = file_get_contents($in);
		$expected = file_get_contents($out);

		$descriptorspec = [
				0 => ["pipe", "r"], // stdin is a pipe that the child will read from
				1 => ["pipe", "w"], // stdout is a pipe that the child will write to
				2 => ["pipe", "w"], // stderr
		];

		$start = microtime(TRUE);
		$process = proc_open('main', $descriptorspec, $pipes, __DIR__, $env = []);

		if (!is_resource($process)) {
			echo "Failed\n";
			die;
		}
		foreach (explode("\n", $cin) as $line) {
			fwrite($pipes[0], "$line\n");
		}
		fclose($pipes[0]);

		$real = stream_get_contents($pipes[1]);
		fclose($pipes[1]);

		$match = [];
		$time = NULL;
		if (preg_match('/~~~(.*?)~~~\n/msi', $real, $match)) {
			$real = preg_replace('/~~~.*?~~~\n/msi', '', $real);

			$return_value = proc_close($process);
			$time = $match[1] * 1000;
		}

		if ($real !== $expected)
		{
			$stats->failed++;
			echo "\033[1;34mFailed on test $in:\033[0m\n";
			echo "$cin\n"; // print test input
			file_put_contents(__DIR__ . '/tests/_real', $real);
			file_put_contents(__DIR__ . '/tests/_expected', $expected);

			$diff = [];
			exec('git diff --minimal --color --no-index tests/_real tests/_expected', $diff);
			$diff = array_slice($diff, 5);
			$diff = implode("\n", $diff);
			echo $diff;

			unlink(__DIR__ . '/tests/_real');
			unlink(__DIR__ . '/tests/_expected');
			$ok = FALSE;
		} else {
			$stats->passed++;
		}

		if ($time !== NULL)
		{
			echo "\n\n\033[1;34mtime:  " . round($time, 4) . " ms\033[0m\n";
		}

		if ($time || $real !== $expected) {
			echo "\n\033[1;34m" . str_repeat('-', 80) . "\033[0m\n\n";
		}

		$total_time += $time;
	}
}

echo "\033[1;34mtotal: " . round($total_time, 4) . " ms\033[0m\n";
if ($ok)
{
	echo "\033[1;32mtests ok\033[0m\n";
}
else
{
	$sum = $stats->passed + $stats->failed;
	echo "\n\033[1;35m#\n# {$stats->passed}/$sum passed\n#\033[0m\n";
}
