import data from './headers.json'
import { kinds } from './kinds'

export type Field = { type: string; name: string; value?: string; doc: string }
export type Method = { signature: string; doc: string; html?: string }
export type Struct = {
  kind: 'struct' | 'class'
  name: string
  template?: string
  templateHtml?: string
  doc: string
  fields: Field[]
  methods: Method[]
}
export type Value = { name: string; doc: string }
export type Enum = { name: string; doc: string; values: Value[] }
export type Alias = { name: string; target: string; doc: string; html?: string }
export type Constant = { type: string; name: string; value: string; doc: string }
export type Fn = { name: string; signature: string; doc: string; html?: string }
export type Header = {
  path: string
  include?: string
  namespace: string
  group: string
  structs: Struct[]
  enums: Enum[]
  aliases: Alias[]
  constants: Constant[]
  functions: Fn[]
}

export const headers = data as Header[]
export { kinds }

export const order = ['Start', 'Edit', 'Document', 'Entities', 'Objects', 'Tables', 'Blocks', 'Audit', 'Values']

export const names = (one: Header): string[] => [
  ...one.structs.map((item) => item.name),
  ...one.enums.map((item) => item.name),
  ...one.aliases.map((item) => item.name),
  ...one.functions.map((item) => item.name),
  ...one.constants.map((item) => item.name),
]
