#include "pch.h"
#include "tasks.h"

namespace mob
{

libloot::libloot()
	: basic_task("libloot")
{
}

std::string libloot::version()
{
	return conf::version_by_name("libloot");
}

std::string libloot::hash()
{
	return conf::version_by_name("libloot_hash");
}

bool libloot::prebuilt()
{
	return false;
}

fs::path libloot::source_path()
{
	return paths::build() / dir_name();
}

void libloot::do_clean(clean c)
{
	instrument<times::clean>([&]
	{
		if (is_set(c, clean::redownload))
			run_tool(downloader(source_url(), downloader::clean));

		if (is_set(c, clean::reextract))
		{
			cx().trace(context::reextract, "deleting {}", source_path());
			op::delete_directory(cx(), source_path(), op::optional);
			return;
		}
	});
}

void libloot::do_fetch()
{
	const auto file = instrument<times::fetch>([&]
	{
		return run_tool(downloader(source_url()));
	});

	instrument<times::extract>([&]
	{
		run_tool(extractor()
			.file(file)
			.output(source_path()));
	});
}

void libloot::do_build_and_install()
{
	instrument<times::install>([&]
	{
		op::copy_file_to_dir_if_better(cx(),
			source_path() / "loot.dll",
			paths::install_loot());
	});
}

std::string libloot::dir_name()
{
	// libloot-0.15.1-0-gf725dd7_0.15.1-win64.7z, yeah
	return
		"libloot-" + version() + "-" + "0-" +
		hash() + "_" + version() + "-" +
		"win64";
}

url libloot::source_url()
{
	return
		"https://github.com/loot/libloot/releases/download/" +
		version() + "/" + dir_name() + ".7z";
}

}	// namespace
