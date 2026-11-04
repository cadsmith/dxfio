import data from './kinds.json'

export type Kind = { type: string; dxf: string; family: 'entity' | 'object' | 'table'; header: string }

export const kinds = data as Kind[]
