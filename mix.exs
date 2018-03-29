defmodule ExNcurses.MixProject do
  use Mix.Project

  def project do
    [
      app: :ex_ncurses,
      version: "0.0.2",
      package: package(),
      elixir: "~> 1.6",
      start_permanent: Mix.env() == :prod,
      deps: deps(),
      compilers: [:elixir_make | Mix.compilers()],
      make_clean: ["clean"]
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger],
      mod: {ExNcurses.Application, []}
    ]
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [{:elixir_make, "~> 0.4", runtime: false}, {:ex_doc, "~> 0.11", only: :dev}]
  end

  defp package() do
    [
      maintainers: [""],
      licenses: ["MIT"],
      links: %{"GitHub" => "https://github.com/jfreeze/ex_ncurses"}
    ]
  end
end
