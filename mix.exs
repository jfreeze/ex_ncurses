defmodule ExNcurses.MixProject do
  use Mix.Project

  def project do
    [
      app: :ex_ncurses,
      version: "0.2.0",
      package: package(),
      elixir: "~> 1.6",
      start_permanent: Mix.env() == :prod,
      description: description(),
      deps: deps(),
      compilers: [:elixir_make | Mix.compilers()],
      make_targets: ["all"],
      make_clean: ["clean"],
      make_env: make_env(),
      aliases: [format: ["format", &format_c/1]]
    ]
  end

  defp make_env() do
    case System.get_env("ERL_EI_INCLUDE_DIR") do
      nil ->
        %{
          "ERL_EI_INCLUDE_DIR" => "#{:code.root_dir()}/usr/include",
          "ERL_EI_LIBDIR" => "#{:code.root_dir()}/usr/lib"
        }

      _ ->
        %{}
    end
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger],
      mod: {ExNcurses.Application, []}
    ]
  end

  defp description() do
    """
    ExNcurses - a NIF interface to the ncurses lib.
    """
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [{:elixir_make, "~> 0.4", runtime: false}, {:ex_doc, "~> 0.11", only: :dev, runtime: false}]
  end

  defp package() do
    [
      name: :ex_ncurses,
      maintainers: ["Jim Freeze, Frank Hunleth, Justin Schneck"],
      licenses: ["MIT"],
      links: %{"GitHub" => "https://github.com/jfreeze/ex_ncurses"},
      files: ["LICENSE", "README.md", "lib/**/*.ex", "mix.exs", "Makefile", "src/*.c"]
    ]
  end

  defp format_c([]) do
    astyle =
      System.find_executable("astyle") ||
        Mix.raise("""
        Could not format C code since astyle is not available.
        """)

    System.cmd(astyle, ["-n", "-r", "src/*.c"], into: IO.stream(:stdio, :line))
  end

  defp format_c(_args), do: true
end
