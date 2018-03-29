defmodule ExNcurses.Server do
  use GenServer
  alias ExNcurses.Nif
  @moduledoc false

  defmodule State do
    @moduledoc false
    defstruct pid: nil
  end

  def start_link(args) do
    GenServer.start_link(__MODULE__, args, name: __MODULE__)
  end

  def invoke(name, args \\ {}) do
    GenServer.call(__MODULE__, {:invoke, name, args})
  end

  def listen() do
    GenServer.call(__MODULE__, :listen)
  end

  def stop_listening() do
    GenServer.call(__MODULE__, :stop_listening)
  end

  def init(_args) do
    {:ok, %State{}}
  end

  def handle_call({:invoke, name, args}, _from, state) do
    {:reply, Nif.invoke(name, args), state}
  end

  def handle_call(:listen, {pid, _ref}, state) do
    {:reply, Nif.poll(), %{state | pid: pid}}
  end

  def handle_call(:stop_listening, _from, state) do
    {:reply, Nif.stop(), %{state | pid: nil}}
  end

  def handle_info({:select, _res, _ref, :ready_input}, state) do
    key = Nif.read()
    maybe_send(state.pid, {:ex_ncurses, :key, key})
    {:noreply, state}
  end

  defp maybe_send(nil, _message), do: :ok
  defp maybe_send(pid, message), do: send(pid, message)
end
